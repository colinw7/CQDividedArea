#ifndef CQDividedArea_H
#define CQDividedArea_H

#include <QFrame>
#include <QToolButton>
#include <QIcon>
#include <map>

class CQDividedAreaWidget;
class CQDividedAreaSplitter;
class CQDividedAreaTitle;
class CQDividedAreaTitleButton;

class CQDividedArea : public QFrame {
  Q_OBJECT

  Q_PROPERTY(bool singleArea READ isSingleArea WRITE setSingleArea)

 public:
  CQDividedArea(QWidget *parent=0);

  bool isSingleArea() const { return singleArea_; }
  void setSingleArea(bool b) { singleArea_ = b; }

  CQDividedAreaWidget *addWidget(QWidget *w, const QString &title, const QIcon &icon=QIcon());

  void removeWidget(QWidget *w);

  QSize minimumSizeHint() const;

  QSize sizeHint() const;

 private:
  friend class CQDividedAreaWidget;

  void showEvent(QShowEvent *e);

  void resizeEvent(QResizeEvent *e);

  void updateLayout(bool reset=true);

 private slots:
  void splitterMoved(int d);

  void widgetCollapseStateChanged(bool collapsed);

 private:
  typedef std::map<int,CQDividedAreaWidget *>   Widgets;
  typedef std::map<int,CQDividedAreaSplitter *> Splitters;

  static int widgetId;

  Widgets   widgets_;
  Splitters splitters_;
  bool      singleArea_ { false };
};

//------

class CQDividedAreaWidget : public QWidget {
  Q_OBJECT

  Q_PROPERTY(QString title          READ title       WRITE setTitle)
  Q_PROPERTY(QIcon   icon           READ icon        WRITE setIcon)
  Q_PROPERTY(bool    collapsed      READ isCollapsed WRITE setCollapsed)
  Q_PROPERTY(int     contentsHeight READ contentsHeight)

 public:
  CQDividedAreaWidget(CQDividedArea *area, int id);

  int id() const { return id_; }

  QWidget *widget() const { return w_; }
  void setWidget(QWidget *w);

  const QString &title() const;
  void setTitle(const QString &title);

  const QIcon &icon() const;
  void setIcon(const QIcon &icon);

  bool isCollapsed() const;
  void setCollapsed(bool collapsed);

  void setTempCollapsed(bool collapsed);

  int titleHeight() const;

  int contentsHeight() const;

  void setContentsHeight(int height);

  int adjustContentsHeight() const { return adjustHeight_; }
  void setAdjustContentsHeight(int height);

  int minContentsHeight() const;

  QSize minimumSizeHint() const;

  QSize sizeHint() const;

 signals:
  void collapseStateChanged(bool);

 private:
  friend class CQDividedArea;

  void collapse();

  void updateState();

 private:
  CQDividedArea*      area_          { nullptr };
  int                 id_            { 0 };
  QWidget*            w_             { nullptr };
  bool                collapsed_     { false };
  bool                tempCollapsed_ { false };
  int                 height_        { -1 };
  int                 adjustHeight_  { 0 };
  CQDividedAreaTitle* titleWidget_   { nullptr };
};

//------

class CQDividedAreaTitle : public QWidget {
  Q_OBJECT

  Q_PROPERTY(QString title    READ title    WRITE setTitle)
  Q_PROPERTY(QIcon   icon     READ icon     WRITE setIcon)
  Q_PROPERTY(QSize   iconSize READ iconSize WRITE setIconSize)

 public:
  CQDividedAreaTitle(CQDividedAreaWidget *widget);

  const QString &title() const { return title_; }
  void setTitle(const QString &title);

  const QIcon &icon() const { return icon_; }
  void setIcon(const QIcon &icon);

  const QSize &iconSize() const { return iconSize_; }
  void setIconSize(const QSize &s);

 private:
  friend class CQDividedAreaWidget;

  void showEvent(QShowEvent *e);

  void resizeEvent(QResizeEvent *e);

  void paintEvent(QPaintEvent *);

  void contextMenuEvent(QContextMenuEvent *);

  void updateLayout();

  void updateState();

 private slots:
  void collapseSlot();

 private:
  CQDividedAreaWidget      *widget_ { nullptr };
  QString                   title_;
  QIcon                     icon_;
  QSize                     iconSize_ { 10, 10 };
  QColor                    bg_;
  CQDividedAreaTitleButton *collapseButton_ { nullptr };
};

//------

class CQDividedAreaTitleButton : public QToolButton {
  Q_OBJECT

 public:
  CQDividedAreaTitleButton(CQDividedAreaTitle *title);

 private:
  void paintEvent(QPaintEvent *);

 private:
  CQDividedAreaTitle *title_ { nullptr };
};

//------

class CQDividedAreaSplitter : public QWidget {
  Q_OBJECT

 public:
  CQDividedAreaSplitter(CQDividedArea *area, int id);

  int id() const { return id_; }

  int otherId() const { return otherId_; }
  void setOtherId(int id) { otherId_ = id; }

 private:
  void mousePressEvent  (QMouseEvent *e);
  void mouseMoveEvent   (QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *e);

  void enterEvent(QEvent *e);
  void leaveEvent(QEvent *e);

  void paintEvent(QPaintEvent *);

 signals:
  void moved(int d);

 private:
  struct MouseState {
    bool   pressed { false };
    QPoint pressPos;

    MouseState() { }
  };

  CQDividedArea *area_ { nullptr };
  int            id_ { 0 };
  int            otherId_ { 0 };
  MouseState     mouseState_;
  bool           mouseOver_ { false };
};

#endif
