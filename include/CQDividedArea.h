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

 public:
  CQDividedArea(QWidget *parent=0);

  int addWidget(QWidget *w, const QString &title);

  QSize minimumSizeHint() const;

 private:
  friend class CQDividedAreaWidget;

  void showEvent(QShowEvent *e);

  void resizeEvent(QResizeEvent *e);

  void updateLayout(bool reset=true);

 private slots:
  void splitterMoved(int d);

 private:
  typedef std::map<int,CQDividedAreaWidget *>   Widgets;
  typedef std::map<int,CQDividedAreaSplitter *> Splitters;

  static int widgetId;

  Widgets   widgets_;
  Splitters splitters_;
};

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

 private:
  friend class CQDividedArea;

  void collapse();

  void updateState();

 private:
  CQDividedArea      *area_;
  int                 id_;
  QWidget            *w_;
  bool                collapsed_;
  bool                tempCollapsed_;
  int                 height_;
  int                 adjustHeight_;
  CQDividedAreaTitle *titleWidget_;
};

class CQDividedAreaTitle : public QWidget {
  Q_OBJECT

 public:
  CQDividedAreaTitle(CQDividedAreaWidget *widget);

  const QString &title() const { return title_; }
  void setTitle(const QString &title);

  const QIcon &icon() const { return icon_; }
  void setIcon(const QIcon &icon);

  const QSize &iconSize() const { return iconSize_; }

 private:
  friend class CQDividedAreaWidget;

  void showEvent(QShowEvent *e);

  void resizeEvent(QResizeEvent *e);

  void paintEvent(QPaintEvent *);

  void updateLayout();

  void updateState();

 private slots:
  void collapseSlot();

 private:
  CQDividedAreaWidget      *widget_;
  QString                   title_;
  QIcon                     icon_;
  QSize                     iconSize_;
  QColor                    bg_;
  CQDividedAreaTitleButton *collapseButton_;
};

class CQDividedAreaTitleButton : public QToolButton {
  Q_OBJECT

 public:
  CQDividedAreaTitleButton(CQDividedAreaTitle *title);

 private:
  void paintEvent(QPaintEvent *);

 private:
  CQDividedAreaTitle *title_;
};

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
    bool   pressed;
    QPoint pressPos;

    MouseState() {
      pressed = false;
    }
  };

  CQDividedArea *area_;
  int            id_;
  int            otherId_;
  MouseState     mouseState_;
  bool           mouseOver_;
};

#endif
