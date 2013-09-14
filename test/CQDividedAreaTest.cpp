#include <CQDividedAreaTest.h>
#include <CQDividedArea.h>

#include <QApplication>
#include <QVBoxLayout>
#include <QPushButton>
#include <QListWidget>

int
main(int argc, char **argv)
{
  QApplication app(argc, argv);

  CQDividedAreaTest *test = new CQDividedAreaTest;

  test->resize(400, 600);

  test->show();

  return app.exec();
}

CQDividedAreaTest::
CQDividedAreaTest(QWidget *parent) :
 QWidget(parent)
{
  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(0);

  CQDividedArea *area = new CQDividedArea;

  QListWidget *list1 = new QListWidget;
  QListWidget *list2 = new QListWidget;
  QListWidget *list3 = new QListWidget;

  list1->addItem("One");
  list1->addItem("Two");
  list1->addItem("Three");

  list2->addItem("Ein");
  list2->addItem("Zwei");
  list2->addItem("Drei");

  list3->addItem("Un");
  list3->addItem("Deuz");
  list3->addItem("Trois");

  area->addWidget(list1, "English");
  area->addWidget(list2, "German" );
  area->addWidget(list3, "French" );

  list1->setMinimumHeight(25);
  list2->setMinimumHeight(50);
  list3->setMinimumHeight(100);

  layout->addWidget(area);
}
