#include <CQDividedArea.h>
#include <CQWidgetUtil.h>

#include <QVBoxLayout>
#include <QPainter>
#include <QStylePainter>
#include <QStyleOption>
#include <QMouseEvent>
#include <QMenu>

#include <cassert>
#include <iostream>

#include <svg/up_gradient_light_svg.h>
#include <svg/up_gradient_dark_svg.h>

#include <svg/down_gradient_light_svg.h>
#include <svg/down_gradient_dark_svg.h>

namespace Constants {
  int MIN_WIDTH  = 32;
  int MIN_HEIGHT = 32;
};

int CQDividedArea::widgetId = 0;

CQDividedArea::
CQDividedArea(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("dividedArea");
}

CQDividedAreaWidget *
CQDividedArea::
addWidget(QWidget *w, const QString &title, const QIcon &icon)
{
  int id = ++widgetId;

  auto *widget = new CQDividedAreaWidget(this, id);

  widget->setWidget(w);
  widget->setTitle (title);
  widget->setIcon  (icon);

  connect(widget, SIGNAL(collapseStateChanged(bool)),
          this, SLOT(widgetCollapseStateChanged(bool)));

  auto *splitter = new CQDividedAreaSplitter(this, id);

  connect(splitter, SIGNAL(moved(int)), this, SLOT(splitterMoved(int)));

  widgets_  [id] = widget;
  splitters_[id] = splitter;

  widget  ->setObjectName(QString("widget_%1"  ).arg(id));
  splitter->setObjectName(QString("splitter_%1").arg(id));

  widget->setVisible(true);

  if (isVisible())
    updateLayout();

  return widget;
}

void
CQDividedArea::
removeWidget(QWidget *w)
{
  for (const auto &idWidget : widgets_) {
    int                  id     = idWidget.first;
    CQDividedAreaWidget *widget = idWidget.second;

    if (widget->widget() != w) continue;

    auto ps = splitters_.find(id);
    assert(ps != splitters_.end());

    CQDividedAreaSplitter *splitter = (*ps).second;

    delete widget;
    delete splitter;

    widgets_  .erase(id);
    splitters_.erase(ps);

    if (isVisible())
      updateLayout();

    return;
  }
}

void
CQDividedArea::
showEvent(QShowEvent *)
{
  updateLayout();
}

void
CQDividedArea::
resizeEvent(QResizeEvent *)
{
  updateLayout();
}

void
CQDividedArea::
updateLayout(bool reset)
{
  int l = contentsMargins().left  ();
  int t = contentsMargins().top   ();
  int r = contentsMargins().right  ();
  int b = contentsMargins().bottom();

  //---

  // get visible widgets
  std::vector<CQDividedAreaWidget *> visibleWidgets;
  int                                numNonVisible = 0;

  for (const auto &idWidget : widgets_) {
    CQDividedAreaWidget *widget = idWidget.second;

    // reset temp collapsed and adjist contents height if needed
    if (reset) {
      widget->setTempCollapsed(false);

      widget->setAdjustContentsHeight(0);
    }

    if (! widget->isCollapsed()) {
      visibleWidgets.push_back(widget);

      numNonVisible = 0;
    }
    else
      ++numNonVisible;
  }

  int numVisible = int(visibleWidgets.size());

  CQDividedAreaWidget *lastWidget = (numVisible ? visibleWidgets.back() : 0);

  int lastSpace = (lastWidget ? numNonVisible*lastWidget->titleHeight() : 0);

  //---

  int adjust   = 0;
  int fixes    = 0;
  int maxFixes = 2*(numVisible + 1);

  while (fixes < maxFixes) {
    int numMinHeight = 0;

    int lastExpandedId = 0;

    int x = l;
    int y = t;
    int w = width() - l - r;
    int h = height() - t - b;

    for (const auto &idWidget : widgets_) {
      int                  id     = idWidget.first;
      CQDividedAreaWidget *widget = idWidget.second;

      bool expanded = ! widget->isCollapsed();

      //---

      CQDividedAreaSplitter *splitter = splitters_[id];

      if (reset)
        splitter->setVisible(false);

      if (expanded && lastExpandedId > 0) {
        CQDividedAreaSplitter *splitter = splitters_[lastExpandedId];

        if (reset) {
          splitter->setOtherId(id);

          splitter->setVisible(true);
        }

        int sh = splitter->height();

        splitter->move  (x, y);
        splitter->resize(w, sh);

        y += sh;
      }

      //---

      int wh = widget->titleHeight();

      if (expanded) {
        if (widget == lastWidget)
          wh = h - y - lastSpace;
        else
          wh += widget->contentsHeight();

        int minH = widget->minContentsHeight();

        if (wh < widget->titleHeight() + minH) {
          wh = widget->titleHeight() + minH;

          ++numMinHeight;
        }
      }

      widget->move  (x, y);
      widget->resize(w, wh);

      if (expanded)
        lastExpandedId = id;

      y += wh;
    }

    if (y <= h || ! reset)
      break;

    if      (numMinHeight < numVisible) {
      adjust = y - h;

      int adjust1 = (numVisible > 1 ? adjust/(numVisible - 1) : 0);

      for (const auto &idWidget : widgets_) {
        CQDividedAreaWidget *widget = idWidget.second;

        if (widget != lastWidget)
          widget->setAdjustContentsHeight(widget->adjustContentsHeight() + adjust1);
      }
    }
    else if (lastWidget) {
      lastWidget->setTempCollapsed(true);

      visibleWidgets.pop_back();

      --numVisible;
      ++numNonVisible;

      lastWidget = (numVisible ? visibleWidgets.back() : 0);

      lastSpace = (lastWidget ? numNonVisible*lastWidget->titleHeight() : 0);
    }
    else
      break;

    ++fixes;
  }
}

void
CQDividedArea::
splitterMoved(int d)
{
  CQDividedAreaSplitter *splitter = qobject_cast<CQDividedAreaSplitter *>(QObject::sender());
  assert(splitter);

  CQDividedAreaWidget *widget1 = widgets_[splitter->id()];      // top
  CQDividedAreaWidget *widget2 = widgets_[splitter->otherId()]; // bottom

  int h1 = widget1->widget()->height() + d;
  int h2 = widget2->widget()->height() - d;

  int minH1 = widget1->minContentsHeight();
  int minH2 = widget2->minContentsHeight();

  if (h1 >= minH1 && h2 >= minH2) {
    widget1->setContentsHeight(h1);
    widget2->setContentsHeight(h2);

    widget1->resize(width(), widget1->titleHeight() + h1);
    widget2->resize(width(), widget2->titleHeight() + h2);

    widget2->move(widget2->x(), widget2->y() + d);

    splitter->move(splitter->x(), splitter->y() + d);
  }

  updateLayout(false);
}

void
CQDividedArea::
widgetCollapseStateChanged(bool)
{
  if (! isSingleArea())
    return;

  CQDividedAreaWidget *widget = qobject_cast<CQDividedAreaWidget *>(sender());
  if (! widget) return;

  for (const auto &idWidget : widgets_) {
    CQDividedAreaWidget *widget = idWidget.second;

    disconnect(widget, SIGNAL(collapseStateChanged(bool)),
               this, SLOT(widgetCollapseStateChanged(bool)));
  }

  if (widget->isCollapsed()) {
    CQDividedAreaWidget *openWidget = nullptr;

    for (const auto &idWidget : widgets_) {
      CQDividedAreaWidget *widget1 = idWidget.second;
      if (widget1 == widget) continue;

      if (! openWidget) {
        widget1->setCollapsed(true);

        openWidget = widget1;
      }
      else
        widget1->setCollapsed(false);
    }
  }
  else {
    for (const auto &idWidget : widgets_) {
      CQDividedAreaWidget *widget1 = idWidget.second;
      if (widget1 == widget) continue;

      widget1->setCollapsed(true);
    }
  }

  for (const auto &idWidget : widgets_) {
    CQDividedAreaWidget *widget = idWidget.second;

    connect(widget, SIGNAL(collapseStateChanged(bool)),
            this, SLOT(widgetCollapseStateChanged(bool)));
  }
}

QSize
CQDividedArea::
minimumSizeHint() const
{
  int l = contentsMargins().left  ();
  int t = contentsMargins().top   ();
  int r = contentsMargins().right  ();
  int b = contentsMargins().bottom();

  int w = 0;
  int h = 0;

  for (const auto &idWidget : widgets_) {
    const CQDividedAreaWidget *widget = idWidget.second;

    w = std::max(w, widget->minimumSizeHint().width());

    h += widget->minimumSizeHint().height();
  }

  return QSize(w + l + r, h + t + b);
}

QSize
CQDividedArea::
sizeHint() const
{
  if (widgets_.empty())
    return QFrame::minimumSizeHint();

  int l = contentsMargins().left  ();
  int t = contentsMargins().top   ();
  int r = contentsMargins().right  ();
  int b = contentsMargins().bottom();

  int w = 0;
  int h = 0;

  for (const auto &idWidget : widgets_) {
    const CQDividedAreaWidget *widget = idWidget.second;

    w = std::max(w, widget->sizeHint().width());

    h += widget->sizeHint().height();
  }

  return QSize(w + l + r, h + t + b);
}

//------

CQDividedAreaWidget::
CQDividedAreaWidget(CQDividedArea *area, int id) :
 QWidget(area), area_(area), id_(id)
{
  setObjectName("widget");

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(0);

  titleWidget_ = new CQDividedAreaTitle(this);

  layout->addWidget(titleWidget_);
}

void
CQDividedAreaWidget::
setWidget(QWidget *w)
{
  assert(! w_);

  w_ = w;

  w_->setParent(this);

  layout()->addWidget(w_);
}

const QString &
CQDividedAreaWidget::
title() const
{
  return titleWidget_->title();
}

void
CQDividedAreaWidget::
setTitle(const QString &title)
{
  titleWidget_->setTitle(title);
}

const QIcon &
CQDividedAreaWidget::
icon() const
{
  return titleWidget_->icon();
}

void
CQDividedAreaWidget::
setIcon(const QIcon &icon)
{
  titleWidget_->setIcon(icon);
}

bool
CQDividedAreaWidget::
isCollapsed() const
{
  return collapsed_ || tempCollapsed_;
}

void
CQDividedAreaWidget::
setCollapsed(bool collapsed)
{
  collapsed_     = collapsed;
  tempCollapsed_ = false;

  updateState();

  area_->updateLayout();

  emit collapseStateChanged(collapsed_);
}

void
CQDividedAreaWidget::
setTempCollapsed(bool collapsed)
{
  tempCollapsed_ = collapsed;

  updateState();
}

void
CQDividedAreaWidget::
updateState()
{
  w_->setVisible(! isCollapsed());

  titleWidget_->updateState();
}

int
CQDividedAreaWidget::
titleHeight() const
{
  return titleWidget_->height();
}

int
CQDividedAreaWidget::
contentsHeight() const
{
  int h = 0;

  if (height_ < 0)
    h = w_->sizeHint().height();
  else
    h = height_;

  h -= adjustHeight_;

  return h;
}

void
CQDividedAreaWidget::
setContentsHeight(int height)
{
  height_ = height;

  adjustHeight_ = 0;
}

void
CQDividedAreaWidget::
setAdjustContentsHeight(int height)
{
  adjustHeight_ = height;
}

int
CQDividedAreaWidget::
minContentsHeight() const
{
  QSize s = CQWidgetUtil::SmartMinSize(w_);

  return std::max(s.height(), Constants::MIN_HEIGHT);
}

QSize
CQDividedAreaWidget::
minimumSizeHint() const
{
  int w = Constants::MIN_WIDTH;
  int h = titleHeight();

  return QSize(w, h);
}

QSize
CQDividedAreaWidget::
sizeHint() const
{
  int w = w_->sizeHint().width();
  int h = w_->sizeHint().height();

  h += titleHeight();

  return QSize(w, h);
}

//------

CQDividedAreaTitle::
CQDividedAreaTitle(CQDividedAreaWidget *widget) :
 widget_(widget)
{
  QFont f = widget->font();

  f.setBold(true);

  setFont(f);

  //---

  setObjectName("title");

  QFontMetrics fm(font());

  setFixedHeight(fm.height() + 2);

  collapseButton_ = new CQDividedAreaTitleButton(this);

  connect(collapseButton_, SIGNAL(clicked()), this, SLOT(collapseSlot()));

  updateState();

  //---

  setContextMenuPolicy(Qt::DefaultContextMenu);
}

void
CQDividedAreaTitle::
setTitle(const QString &title)
{
  title_ = title;

  update();
}

void
CQDividedAreaTitle::
setIcon(const QIcon &icon)
{
  icon_ = icon;

  update();
}

void
CQDividedAreaTitle::
setIconSize(const QSize &s)
{
  iconSize_ = s;
}

void
CQDividedAreaTitle::
showEvent(QShowEvent *)
{
  updateLayout();
}

void
CQDividedAreaTitle::
resizeEvent(QResizeEvent *)
{
  updateLayout();
}

void
CQDividedAreaTitle::
updateLayout()
{
  collapseButton_->move(width() - collapseButton_->width() - 2,
                        (height() - collapseButton_->height())/2);
}

void
CQDividedAreaTitle::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  bg_ = widget_->palette().window().color().darker(110);

  painter.fillRect(rect(), QBrush(bg_));

  int x = 2;
  int h = height();

  int iw = 0;

  if (! icon_.isNull()) {
    iw = iconSize().width() + 2;

    painter.drawPixmap(x, (h - iconSize().height())/2, icon().pixmap(iconSize()));

    x += iw;
  }

  int bw = collapseButton_->width();

  if (title_.length()) {
    QFontMetrics fm(font());

    QString title = fm.elidedText(title_, Qt::ElideRight, width() - iw - bw - 6);

    // if just an ellipsis keep first letter
    if (title.length() == 0 || title.utf16()[0] == 8230)
      title = title_[0] + "..";

    painter.drawText(x, (h - fm.height())/2 + fm.ascent(), title);
  }

  QRect buttonRect(collapseButton_->x() - 2, 0, bw + 4, height());

  painter.fillRect(buttonRect, QBrush(bg_));

  QColor c = widget_->palette().window().color().darker(200);

  painter.setPen(c);

  painter.drawLine(0, height() - 1, width() - 1, height() - 1);
}

void
CQDividedAreaTitle::
contextMenuEvent(QContextMenuEvent *e)
{
  QMenu *menu = new QMenu;

  QAction *collapseAction = menu->addAction(widget_->isCollapsed() ? "Expand" : "Collapse");

  menu->addAction(collapseAction);

  connect(collapseAction, SIGNAL(triggered()), this, SLOT(collapseSlot()));

  menu->exec(e->globalPos());

  delete menu;
}

void
CQDividedAreaTitle::
collapseSlot()
{
  widget_->setCollapsed(! widget_->isCollapsed());
}

void
CQDividedAreaTitle::
updateState()
{
  if (widget_->isCollapsed()) {
    collapseButton_->setIcon("UP_GRADIENT");

    collapseButton_->setToolTip("Expand");
  }
  else {
    collapseButton_->setIcon("DOWN_GRADIENT");

    collapseButton_->setToolTip("Collapse");
  }
}

//------

CQDividedAreaTitleButton::
CQDividedAreaTitleButton(CQDividedAreaTitle *title) :
 CQIconButton(title), title_(title)
{
  setObjectName("button");

  setSize(Size::SMALL);

  //setIconSize(title_->iconSize());
  //int is = 0.9*QFontMetrics(font()).ascent();
  //setIconSize(QSize(is, is));

  setAutoRaise(true);

  setFocusPolicy(Qt::NoFocus);

  setCursor(Qt::ArrowCursor);
}

void
CQDividedAreaTitleButton::
paintEvent(QPaintEvent *)
{
  QStylePainter p(this);

  QStyleOptionToolButton opt;

  initStyleOption(&opt);

  p.drawComplexControl(QStyle::CC_ToolButton, opt);
}

//------

CQDividedAreaSplitter::
CQDividedAreaSplitter(CQDividedArea *area, int id) :
 QWidget(area), area_(area), id_(id)
{
  setObjectName("splitter");

  setFixedHeight(5);

  setCursor(Qt::SizeVerCursor);

  setMouseTracking(true);
}

void
CQDividedAreaSplitter::
paintEvent(QPaintEvent *)
{
  QStylePainter ps(this);

  QStyleOption opt;

  opt.initFrom(this);

  opt.rect  = rect();
//opt.state = (orient_ == Qt::Horizontal ? QStyle::State_None : QStyle::State_Horizontal);
  opt.state = QStyle::State_None;

  if (mouseState_.pressed)
    opt.state |= QStyle::State_Sunken;

  if (mouseOver_)
    opt.state |= QStyle::State_MouseOver;

  ps.drawControl(QStyle::CE_Splitter, opt);
}

void
CQDividedAreaSplitter::
mousePressEvent(QMouseEvent *e)
{
  mouseState_.pressed  = true;
  mouseState_.pressPos = e->globalPos();

  update();
}

void
CQDividedAreaSplitter::
mouseMoveEvent(QMouseEvent *e)
{
  if (! mouseState_.pressed) return;

  int dy = e->globalPos().y() - mouseState_.pressPos.y();

  if (dy) {
    //move(pos().x(), pos().y() + dy);

    emit moved(dy);

    mouseState_.pressPos = e->globalPos();
  }

  update();
}

void
CQDividedAreaSplitter::
mouseReleaseEvent(QMouseEvent *)
{
  mouseState_.pressed = false;

  update();
}

void
CQDividedAreaSplitter::
enterEvent(QEvent *)
{
  mouseOver_ = true;

  update();
}

void
CQDividedAreaSplitter::
leaveEvent(QEvent *)
{
  mouseOver_ = false;

  update();
}
