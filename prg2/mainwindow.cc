// The main program (provided by the course), TIE-20100/TIE-20106
//
// DO ****NOT**** EDIT THIS FILE!
// (Preferably do not edit this even temporarily. And if you still decide to do so
//  (for debugging, for example), DO NOT commit any changes to git, but revert all
//  changes later. Otherwise you won't be able to get any updates/fixes to this
//  file from git!)

#include "mainprogram.hh"

// Qt generated main window code

#ifdef GRAPHICAL_GUI
#include <QCoreApplication>
#include <QFileDialog>
#include <QDir>
#include <QFont>
#include <QGraphicsItem>
#include <QColor>
#include <QBrush>
#include <QPen>
#include <QGraphicsItem>
#include <QVariant>

#include <string>
using std::string;

#include <sstream>
using std::ostringstream;

#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <utility>
#include <tuple>

#include <cassert>

#include "mainwindow.hh"
#include "ui_mainwindow.h"

// Needed to be able to store StopID in QVariant (in QGraphicsItem)
// (Not really necessary since StopID is currently int)
Q_DECLARE_METATYPE(StopID)
// The same for RegionIS (currently a string)
Q_DECLARE_METATYPE(RegionID)

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mainprg_.setui(this);

    // Execute line
    connect(ui->execute_button, &QPushButton::pressed, this, &MainWindow::execute_line);

    // Line edit
    connect(ui->lineEdit, &QLineEdit::returnPressed, this, &MainWindow::execute_line);

    // File selection
    connect(ui->file_button, &QPushButton::pressed, this, &MainWindow::select_file);

    // Command selection
    // !!!!! Sort commands in alphabetical order (should not be done here, but is)
    std::sort(mainprg_.cmds_.begin(), mainprg_.cmds_.end(), [](auto& l, auto& r){ return l.cmd < r.cmd; });
    for (auto& cmd : mainprg_.cmds_)
    {
        ui->cmd_select->addItem(QString::fromStdString(cmd.cmd));
    }
    connect(ui->cmd_select, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated), this, &MainWindow::cmd_selected);

    // Number selection
    for (auto i = 0; i <= 20; ++i)
    {
        ui->number_select->addItem(QString("%1").arg(i));
    }
    connect(ui->number_select, static_cast<void(QComboBox::*)(QString const&)>(&QComboBox::activated), this, &MainWindow::number_selected);

    // Output box
    QFont monofont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    monofont.setStyleHint(QFont::TypeWriter);
    ui->output->setFont(monofont);

    // Initialize graphics scene & view
    gscene_ = new QGraphicsScene(this);
    ui->graphics_view->setScene(gscene_);
    ui->graphics_view->resetTransform();
    ui->graphics_view->setBackgroundBrush(Qt::black);

    // Selecting graphics items by mouse
    connect(gscene_, &QGraphicsScene::selectionChanged, this, &MainWindow::scene_selection_change);
//    connect(this, &MainProgram::signal_clear_selection, this, &MainProgram::clear_selection);

    // Zoom slider changes graphics view scale
    connect(ui->zoom_plus, &QToolButton::clicked, [this]{ this->ui->graphics_view->scale(1.1, 1.1); });
    connect(ui->zoom_minus, &QToolButton::clicked, [this]{ this->ui->graphics_view->scale(1/1.1, 1/1.1); });
    connect(ui->zoom_1, &QToolButton::clicked, [this]{ this->ui->graphics_view->resetTransform(); });
    connect(ui->zoom_fit, &QToolButton::clicked, this, &MainWindow::fit_view);

    // Changing checkboxes updates view
    connect(ui->routes_checkbox, &QCheckBox::clicked, this, &MainWindow::update_view);
    connect(ui->routeids_checkbox, &QCheckBox::clicked, this, &MainWindow::update_view);
    connect(ui->stops_checkbox, &QCheckBox::clicked, this, &MainWindow::update_view);
    connect(ui->stopnames_checkbox, &QCheckBox::clicked, this, &MainWindow::update_view);
    connect(ui->regions_checkbox, &QCheckBox::clicked, this, &MainWindow::update_view);
    connect(ui->regionnames_checkbox, &QCheckBox::clicked, this, &MainWindow::update_view);

    // Unchecking routes checkbox disables route IDs
    connect(ui->routes_checkbox, &QCheckBox::clicked,
            [this]{ this->ui->routeids_checkbox->setEnabled(this->ui->routes_checkbox->isChecked()); });

    // Unchecking stops checkbox disables stop names checkbox
    connect(ui->stops_checkbox, &QCheckBox::clicked,
            [this]{ this->ui->stopnames_checkbox->setEnabled(this->ui->stops_checkbox->isChecked()); });

    // Unchecking regions checkbox disables region names checkbox
    connect(ui->regions_checkbox, &QCheckBox::clicked,
            [this]{ this->ui->regionnames_checkbox->setEnabled(this->ui->regions_checkbox->isChecked()); });

    // Changing font or points scale updates view
    connect(ui->fontscale, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::update_view);
    connect(ui->pointscale, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::update_view);

    // Clear input button
    connect(ui->clear_input_button, &QPushButton::clicked, this, &MainWindow::clear_input_line);

    // Stop button
    connect(ui->stop_button, &QPushButton::clicked, [this](){ this->stop_pressed_ = true; });

    clear_input_line();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::update_view()
{
//    ui->output->appendPlainText("Update view:");

    gscene_->clear();
    auto pointscale = ui->pointscale->value();
    auto fontscale = ui->fontscale->value();
    bool errors = false;
    std::ostringstream errorout;

    if (ui->stops_checkbox->isChecked() || ui->routes_checkbox->isChecked())
    {
        auto stops = mainprg_.ds_.all_stops();
        if (!errors && stops.size() == 1 && stops.front() == NO_STOP)
        {
            errorout << "GUI error: all_stops() returned error {NO_STOP}" << std::endl;
            errors = true;
        }

        std::unordered_map<StopID, std::string> result_stops;
        std::map<std::pair<StopID, StopID>, std::set<RouteID>> journey_result; // Should be unordered_map, but std::pair has no hash()
        if (mainprg_.prev_result.first == MainProgram::ResultType::STOPIDLIST)
        {
            // Copy the stop id vector to the result set
            auto& prev_result = std::get<MainProgram::CmdResultStopIDs>(mainprg_.prev_result.second);
            int i = 0;
            std::for_each(prev_result.second.begin(), prev_result.second.end(),
                          [&result_stops, &i](auto id){ result_stops[id] += MainProgram::convert_to_string(++i)+". "; });
        }
        else if (mainprg_.prev_result.first == MainProgram::ResultType::JOURNEY || mainprg_.prev_result.first == MainProgram::ResultType::ROUTES)
        {
            auto& prev_result = std::get<MainProgram::CmdResultJourney>(mainprg_.prev_result.second);
            int i = 1;
            for (auto& conn : prev_result)
            {
                auto res = result_stops.emplace(std::get<0>(conn), MainProgram::convert_to_string(i)+". ");
                if (!res.second) { res.first->second.append(MainProgram::convert_to_string(i)+". "); }
                ++i;
            }

            std::for_each(prev_result.begin(), prev_result.end(), [&journey_result](auto conn){ journey_result[{std::get<0>(conn), std::get<1>(conn)}].insert(std::get<2>(conn)); });
        }


        for (auto stopid : stops)
        {
            QColor stopcolor = Qt::white;
            QColor namecolor = Qt::cyan;
            QColor stopborder = Qt::white;
            int stopzvalue = 1;

            auto xy = mainprg_.ds_.get_stop_coord(stopid);
            auto [x,y] = xy;
            if (!errors && (x == NO_VALUE || y == NO_VALUE))
            {
                errorout << "GUI error: get_stop_coord(" << stopid << ") returned error (";
                if (xy == NO_COORD)
                {
                    errorout << "NO_COORD";
                }
                else
                {
                    if (x == NO_VALUE) { errorout << "NO_VALUE"; } else { errorout << x; }
                    errorout << ",";
                    if (y == NO_VALUE) { errorout << "NO_VALUE"; } else { errorout << y; }
                }
                errorout << ")" << std::endl;
                errors = true;
            }

            if (x == NO_VALUE || y == NO_VALUE)
            {
                x = 0; y = 0;
                stopcolor = Qt::magenta;
                namecolor = Qt::magenta;
                stopzvalue = 30;
            }

            string prefix;
            auto res_place = result_stops.find(stopid);
            if (res_place != result_stops.end())
            {
                if (result_stops.size() > 1) { prefix = res_place->second; }
                namecolor = Qt::red;
                stopborder = Qt::red;
                stopzvalue = 2;
            }

            if (ui->stops_checkbox->isChecked())
            {
                auto groupitem = gscene_->createItemGroup({});
                groupitem->setFlag(QGraphicsItem::ItemIsSelectable);
                groupitem->setData(0, QVariant::fromValue(stopid));

                QPen stoppen(stopborder);
                stoppen.setWidth(0); // Cosmetic pen
                auto dotitem = gscene_->addEllipse(-4*pointscale, -4*pointscale, 8*pointscale, 8*pointscale,
                                                   stoppen, QBrush(stopcolor));
                dotitem->setFlag(QGraphicsItem::ItemIgnoresTransformations);
                groupitem->addToGroup(dotitem);
                //        dotitem->setFlag(QGraphicsItem::ItemIgnoresTransformations);
                //        dotitem->setData(0, QVariant::fromValue(town));

                // Draw stop names
                string label = prefix;
                if (ui->stopnames_checkbox->isChecked())
                {
                    auto name = mainprg_.ds_.get_stop_name(stopid);
                    if (!errors && name == NO_NAME)
                    {
                        errorout << "GUI error: get_stop_name(" << stopid << ") returned error {NO_NAME}" << std::endl;
                        errors = true;
                    }

                    label += name;
                }

                if (!label.empty())
                {
                    // Create extra item group to be able to set ItemIgnoresTransformations on the correct level (addSimpleText does not allow
                    // setting initial coordinates in item coordinates
                    auto textgroupitem = gscene_->createItemGroup({});
                    auto textitem = gscene_->addSimpleText(QString::fromStdString(label));
                    auto font = textitem->font();
                    font.setPointSizeF(font.pointSizeF()*fontscale);
                    textitem->setFont(font);
                    textitem->setBrush(QBrush(namecolor));
                    textitem->setPos(-textitem->boundingRect().width()/2, -4*pointscale - textitem->boundingRect().height());
                    textgroupitem->addToGroup(textitem);
                    textgroupitem->setFlag(QGraphicsItem::ItemIgnoresTransformations);
                    groupitem->addToGroup(textgroupitem);
                }

                groupitem->setPos(20*x, -20*y);
                groupitem->setZValue(stopzvalue);
            }

            // Draw connections
            if (ui->routes_checkbox->isChecked())
            {
                auto groupitem = gscene_->createItemGroup({});

                auto connlist = mainprg_.ds_.routes_from(stopid);
                if (!errors && connlist.size() == 1 && (connlist.front().first == NO_ROUTE && connlist.front().second == NO_STOP))
                {
                    errorout << "GUI error: get_routes_from(" << stopid << ") returned error {NO_ROUTE , NO_STOP}" << std::endl;
                    errors = true;
                }

                // Combine connections for printing the route IDs
                std::unordered_map<StopID, std::set<RouteID>> connections;
                std::for_each(connlist.begin(), connlist.end(), [&connections](auto conn){ connections[conn.second].insert(conn.first); });

                for (auto& conn : connections)
                {
                    auto stopid2 = conn.first;
                    auto xy2 = mainprg_.ds_.get_stop_coord(stopid2);
                    auto [rx, ry] = xy2;
                    if (!errors && (rx == NO_VALUE || ry == NO_VALUE))
                    {
                        errorout << "GUI error: get_stop_coord(" << stopid2 << ") returned error (";
                        if (xy2 == NO_COORD)
                        {
                            errorout << "NO_COORD";
                        }
                        else
                        {
                            if (rx == NO_VALUE) { errorout << "NO_VALUE"; } else { errorout << rx; }
                            errorout << ",";
                            if (ry == NO_VALUE) { errorout << "NO_VALUE"; } else { errorout << ry; }
                        }
                        errorout << ")" << std::endl;
                        errors = true;
                    }

                    // Create connection name string
                    auto& routes = conn.second;
                    bool walk_only = (routes.size() == 1 && *(routes.begin()) == NO_ROUTE);
                    std::string connstr;
                    if (!walk_only)
                    {
                        std::for_each(routes.begin(), routes.end(),
                                      [&connstr](auto routeid){ connstr.append(((!connstr.empty()) ? "," : "") + ((routeid == NO_ROUTE) ? "-" : routeid)); });
                    }

                    QColor linecolor = Qt::gray;
                    int zvalue = -2;

                    string resstr;
                    if (mainprg_.prev_result.first == MainProgram::ResultType::JOURNEY || mainprg_.prev_result.first == MainProgram::ResultType::ROUTES)
                    {
                        auto res_place2 = journey_result.find({stopid, stopid2});

                        if (res_place2 != journey_result.end())
                        {
                            std::for_each(res_place2->second.begin(), res_place2->second.end(),
                                          [&resstr](auto routeid){ resstr.append(((!resstr.empty()) ? "," : "") + ((routeid == NO_ROUTE) ? "-" : routeid));});
                            if (res_place2->second.size() == 1 && *(res_place2->second.begin()) == NO_ROUTE)
                            {
                                walk_only = true;
                            }

                            linecolor = Qt::red;
                            zvalue = 10;
                        }
                    }

                    if (xy2 == NO_COORD)
                    {
                        rx = 0; ry = 0;
                    }

                    if (xy == NO_COORD || xy2 == NO_COORD)
                    {
                        linecolor = Qt::green;
                    }

                    auto pen = QPen(linecolor);
                    pen.setWidth(0); // "Cosmetic" pen
                    if (walk_only) { pen.setDashPattern({1, 4}); }
                    QLineF line(QPointF(20*rx, -20*ry), QPointF(20*x, -20*y));
                    auto lineitem = gscene_->addLine(line, pen);
                    lineitem->setZValue(zvalue);

                    double const PI  = 3.141592653589793238463;
                    auto arrowSize = 10*pointscale;

                    double angle = std::atan2(-line.dy(), line.dx());

                    QPointF arrowP1 = /*line.p1() +*/ QPointF(sin(angle + PI / 3) * arrowSize,
                                                    cos(angle + PI / 3) * arrowSize);
                    QPointF arrowP2 = /*line.p1() +*/ QPointF(sin(angle + PI - PI / 3) * arrowSize,
                                                    cos(angle + PI - PI / 3) * arrowSize);

                    QPolygonF arrowHead;
                    arrowHead << QPointF(0.0, 0.0) << arrowP1 << arrowP2;
                    auto headitem = gscene_->addPolygon(arrowHead, pen, QBrush(pen.color()));
                    headitem->setPos(line.p1());
                    headitem->setFlag(QGraphicsItem::ItemIgnoresTransformations);
                    headitem->setZValue(zvalue);

                    if ((ui->routeids_checkbox->isChecked() && !connstr.empty()) || !resstr.empty())
                    {
                        string label = resstr;
                        if (ui->routeids_checkbox->isChecked() && connstr != resstr)
                        {
                            if (resstr.empty())
                            {
                                label = connstr;
                            }
                            else
                            {
                                label += (!connstr.empty()) ? ("("+connstr+")") : "";
                            }
                        }
                        // Create extra item group to be able to set ItemIgnoresTransformations on the correct level (addSimpleText does not allow
                        // setting initial coordinates in item coordinates
                        auto textgroupitem = gscene_->createItemGroup({});
                        auto textitem = gscene_->addSimpleText(QString::fromStdString(label));
                        textitem->setBrush(QBrush(linecolor));
                        auto font = textitem->font();
                        font.setPointSizeF(font.pointSizeF()*fontscale);
                        textitem->setFont(font);
                        // Position text to the right side of the arrow
                        QPointF tpos = QPointF(sin(angle + PI) * 0.3 * arrowSize,
                                                               cos(angle + PI) * 0.3 * arrowSize);
//                        auto tickitem = gscene_->addLine(QLineF(line.center(), line.center() + tpos), pen);
//                        tickitem->setZValue(zvalue);


                        // Adjust position based on angle
                        QPointF ttpos;
                        auto bbox = textitem->boundingRect();
                        if (angle < -PI/2)
                        {
                            // Left top is ok, for angle >= -PI && angle < -PI/2
                        }
                        else if (angle < 0)
                        {
                            ttpos = QPointF(0, -bbox.height());
                        }
                        else if (angle < PI/2)
                        {
                            ttpos = QPointF(-bbox.width(), -bbox.height());
                        }
                        else // angle >= PI/2 && angle < PI
                        {
                            ttpos = QPointF(-bbox.width(), 0);
                        }

                        textitem->setPos(ttpos);
                        textitem->setZValue(zvalue);
                        textgroupitem->addToGroup(textitem);
                        textgroupitem->setPos(line.center()+tpos);
                        textgroupitem->setFlag(QGraphicsItem::ItemIgnoresTransformations);
                        groupitem->addToGroup(textgroupitem);
                    }
                }
            }
        }
    }

    // Draw regions
    if (ui->regions_checkbox->isChecked())
    {
        std::unordered_set<RegionID> resultregions;
        if (mainprg_.prev_result.first == MainProgram::ResultType::STOPIDLIST)
        {
            resultregions.insert(std::get<MainProgram::CmdResultStopIDs>(mainprg_.prev_result.second).first);
        }
        else if (mainprg_.prev_result.first == MainProgram::ResultType::REGIONIDLIST)
        {
            auto& prevresult = std::get<MainProgram::CmdResultRegionIDs>(mainprg_.prev_result.second);
            resultregions.insert(prevresult.begin(), prevresult.end());
        }

        auto regionids = mainprg_.ds_.all_regions();
        if (!errors && regionids.size() == 1 && regionids.front() == NO_REGION)
        {
            errorout << "GUI error: all_regions() returned error {NO_REGION}" << std::endl;
            errors = true;
        }

        for (auto regionid : regionids)
        {
            QColor bboxcolor = Qt::blue;
            int bboxzvalue = -3;

            if (regionid != NO_REGION)
            {
                if (resultregions.find(regionid) != resultregions.end())
                {
                    bboxcolor = Qt::green;
                    bboxzvalue = -2;
                }
                auto [min, max] = mainprg_.ds_.region_bounding_box(regionid);
                if (!errors && !(min == NO_COORD && max == NO_COORD) &&
                    (min.x == NO_VALUE || min.y == NO_VALUE || max.x == NO_VALUE || max.y == NO_VALUE))
                {
                    errorout << "GUI error: region_bounding_box(" << regionid << ") returned error {(";
                    if (min == NO_COORD)
                    {
                        errorout << "NO_COORD";
                    }
                    else
                    {
                        if (min.x == NO_VALUE) { errorout << "NO_VALUE"; } else { errorout << min.x; }
                        errorout << ",";
                        if (min.y == NO_VALUE) { errorout << "NO_VALUE"; } else { errorout << min.y; }
                    }
                    errorout << "), (";
                    if (max == NO_COORD)
                    {
                        errorout << "NO_COORD";
                    }
                    else
                    {
                        if (max.x == NO_VALUE) { errorout << "NO_VALUE"; } else { errorout << max.x; }
                        errorout << ",";
                        if (max.y == NO_VALUE) { errorout << "NO_VALUE"; } else { errorout << max.y; }
                    }
                    errorout << ")}" << std::endl;
                    errors = true;
                }

                if (min != NO_COORD && max != NO_COORD)
                {
                    auto pen = QPen(bboxcolor);
                    pen.setWidth(0); // "Cosmetic" pen
                    auto lineitem = gscene_->addLine(20*min.x, -20*min.y, 20*min.x, -20*max.y, pen);
                    lineitem->setZValue(bboxzvalue);
                    lineitem = gscene_->addLine(20*min.x, -20*max.y, 20*max.x, -20*max.y, pen);
                    lineitem->setZValue(bboxzvalue);
                    lineitem = gscene_->addLine(20*max.x, -20*max.y, 20*max.x, -20*min.y, pen);
                    lineitem->setZValue(bboxzvalue);
                    lineitem = gscene_->addLine(20*max.x, -20*min.y, 20*min.x, -20*min.y, pen);
                    lineitem->setZValue(bboxzvalue);

                    if (ui->regionnames_checkbox->isChecked())
                    {
                        // Create extra item group to be able to set ItemIgnoresTransformations on the correct level (addSimpleText does not allow
                        // setting initial coordinates in item coordinates
                        auto textgroupitem = gscene_->createItemGroup({});
                        auto name = mainprg_.ds_.get_region_name(regionid);
                        if (!errors && name == NO_NAME)
                        {
                            errorout << "GUI error: get_region_name(" << regionid << ") returned error {NO_NAME}" << std::endl;
                            errors = true;
                        }

                        auto textitem = gscene_->addSimpleText(QString::fromStdString(name));
                        textitem->setBrush(QBrush(bboxcolor));
                        auto font = textitem->font();
                        font.setPointSizeF(font.pointSizeF()*fontscale);
                        textitem->setFont(font);
                        textitem->setPos(-textitem->boundingRect().width()/2, -25*fontscale);
                        textgroupitem->addToGroup(textitem);
                        textgroupitem->setPos(20*(min.x+max.x)/2, -20*max.y);
                        textgroupitem->setFlag(QGraphicsItem::ItemIgnoresTransformations);
                        textgroupitem->setFlag(QGraphicsItem::ItemIsSelectable);
                        textgroupitem->setData(0, QVariant::fromValue(regionid));
                        textgroupitem->setZValue(bboxzvalue);
                    }
                }
            }
        }
    }

    if (errors)
    {
        output_text(errorout);
        output_text_end();
    }
}

void MainWindow::output_text(ostringstream& output)
{
    string outstr = output.str();
    if (!outstr.empty())
    {
        if (outstr.back() == '\n') { outstr.pop_back(); } // Remove trailing newline
        ui->output->appendPlainText(QString::fromStdString(outstr));
        ui->output->ensureCursorVisible();
        ui->output->repaint();
    }

    output.str(""); // Clear the stream, because it has already been output
}

void MainWindow::output_text_end()
{
    ui->output->moveCursor(QTextCursor::End);
    ui->output->ensureCursorVisible();
    ui->output->repaint();
}

bool MainWindow::check_stop_pressed() const
{
    QCoreApplication::processEvents();
    return stop_pressed_;
}

void MainWindow::execute_line()
{
    auto line = ui->lineEdit->text();
    clear_input_line();
    ui->output->appendPlainText(QString::fromStdString(MainProgram::PROMPT)+line);

    ui->execute_button->setEnabled(false);
    ui->stop_button->setEnabled(true);
    stop_pressed_ = false;

    ostringstream output;
    bool cont = mainprg_.command_parse_line(line.toStdString(), output);
    ui->lineEdit->clear();
    output_text(output);
    output_text_end();

    ui->stop_button->setEnabled(false);
    ui->execute_button->setEnabled(true);
    stop_pressed_ = false;

    ui->lineEdit->setFocus();

//    if (mainprg_.view_dirty)
//    {
//        update_view();
//        mainprg_.view_dirty = false;
//    }
    update_view();

    if (!cont)
    {
        close();
    }
}

void MainWindow::cmd_selected(int idx)
{
    ui->lineEdit->insert(QString::fromStdString(mainprg_.cmds_[idx].cmd+" "));
    ui->cmd_info_text->setText(QString::fromStdString(mainprg_.cmds_[idx].cmd+" "+mainprg_.cmds_[idx].info));

    ui->lineEdit->setFocus();
}

void MainWindow::number_selected(QString number)
{
    ui->lineEdit->insert(number+" ");

    ui->lineEdit->setFocus();
}

void MainWindow::select_file()
{
    QFileDialog dialog(this, "Select file", QDir::currentPath(), "Command list (*.txt)");
    dialog.setFileMode(QFileDialog::ExistingFiles);
    if (dialog.exec())
    {
        auto filenames = dialog.selectedFiles();
        for (auto& i : filenames)
        {
            auto filename = QDir("").relativeFilePath(i);
            ui->lineEdit->insert("\""+filename+"\" ");
        }
    }

    ui->lineEdit->setFocus();
}

void MainWindow::clear_input_line()
{
    ui->cmd_info_text->clear();
    ui->lineEdit->clear();
    ui->lineEdit->setFocus();
}

void MainWindow::fit_view()
{
    ui->graphics_view->fitInView(gscene_->itemsBoundingRect(), Qt::KeepAspectRatio);
}

void MainWindow::scene_selection_change()
{
    auto items = gscene_->selectedItems();
    if (!items.empty())
    {
        for (auto i : items)
        {
            auto data = i->data(0);
//            assert(stopid!=NO_ID && "NO_ID as stop in graphics view");
            if (!selection_clear_in_progress)
            {
                ostringstream output;
                output << "*click* ";
                std::string ui_str;
                if (data.canConvert<StopID>())
                {
                    auto stopid = data.value<StopID>();
                    ui_str = mainprg_.print_stop(stopid, output);
                }
                else if (data.canConvert<RegionID>())
                {
                    auto regionid = data.value<RegionID>();
                    ui_str = mainprg_.print_region(regionid, output);
                }
                if (!ui_str.empty()) { ui->lineEdit->insert(QString::fromStdString(ui_str+" ")); }
                output_text(output);
                output_text_end();
            }
            i->setSelected(false);
            selection_clear_in_progress = !selection_clear_in_progress;
        }
    }
}

void MainWindow::clear_selection()
{
    gscene_->clearSelection();
}



// Originally in main.cc
#include <QApplication>

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        QApplication a(argc, argv);
        MainWindow w;
        w.show();

        auto status = a.exec();
        std::cerr << "Program ended normally." << std::endl;
        return status;
    }
    else
    {
        return MainProgram::mainprogram(argc, argv);
    }
}

#else

int main(int argc, char *argv[])
{
    return MainProgram::mainprogram(argc, argv);
}
#endif
