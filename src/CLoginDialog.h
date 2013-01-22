#ifndef CLOGINDIALOG_H

#include <QDialog>
 
class QLabel;
class QLineEdit;
class QDialogButtonBox;

class CLoginDialog : public QDialog
{
  Q_OBJECT
 
  public:
    explicit CLoginDialog(QWidget *parent, const QString& username, const QString& serverName);

    QString username(void);
    QString password(void);

  private:
    QLabel* labelUsername;
    QLabel* labelPassword;
    QLineEdit* editUsername;
    QLineEdit* editPassword;
    QDialogButtonBox* buttons;
 
    void setUpGUI();
};
 
#endif // CLOGINDIALOG_H
