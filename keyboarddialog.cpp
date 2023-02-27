#include "keyboarddialog.h"
#include "ui_keyboarddialog.h"

KeyboardDialog::KeyboardDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::KeyboardDialog)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose,true);
    connect ( ui->Buttonq, SIGNAL( pressed() ), this, SLOT( keyPressHandler() ) );
    connect ( ui->Buttonw, SIGNAL( pressed() ), this, SLOT( keyPressHandler() ) );
    connect ( ui->Buttone, SIGNAL( pressed() ), this, SLOT( keyPressHandler() ) );
    connect ( ui->Buttonr, SIGNAL( pressed() ), this, SLOT( keyPressHandler() ) );
    connect ( ui->Buttont, SIGNAL( pressed() ), this, SLOT( keyPressHandler() ) );
    connect ( ui->Buttony, SIGNAL( pressed() ), this, SLOT( keyPressHandler() ) );
    connect ( ui->Buttonu, SIGNAL( pressed() ), this, SLOT( keyPressHandler() ) );
    connect ( ui->Buttoni, SIGNAL( pressed() ), this, SLOT( keyPressHandler() ) );
    connect ( ui->Buttono, SIGNAL( pressed() ), this, SLOT( keyPressHandler() ) );
    connect ( ui->Buttonp, SIGNAL( pressed() ), this, SLOT( keyPressHandler() ) );


    connect ( ui->Buttona, SIGNAL( pressed() ), this, SLOT( keyPressHandler() ) );
    connect ( ui->Buttons, SIGNAL( pressed() ), this, SLOT( keyPressHandler() ) );
    connect ( ui->Buttond, SIGNAL( pressed() ), this, SLOT( keyPressHandler() ) );
    connect ( ui->Buttonf, SIGNAL( pressed() ), this, SLOT( keyPressHandler() ) );
    connect ( ui->Buttong, SIGNAL( pressed() ), this, SLOT( keyPressHandler() ) );
    connect ( ui->Buttonh, SIGNAL( pressed() ), this, SLOT( keyPressHandler() ) );
    connect ( ui->Buttonj, SIGNAL( pressed() ), this, SLOT( keyPressHandler() ) );
    connect ( ui->Buttonk, SIGNAL( pressed() ), this, SLOT( keyPressHandler() ) );
    connect ( ui->Buttonl, SIGNAL( pressed() ), this, SLOT( keyPressHandler() ) );

    connect ( ui->Buttonz, SIGNAL( pressed() ), this, SLOT( keyPressHandler() ) );
    connect ( ui->Buttonx, SIGNAL( pressed() ), this, SLOT( keyPressHandler() ) );
    connect ( ui->Buttonc, SIGNAL( pressed() ), this, SLOT( keyPressHandler() ) );
    connect ( ui->Buttonv, SIGNAL( pressed() ), this, SLOT( keyPressHandler() ) );
    connect ( ui->Buttonb, SIGNAL( pressed() ), this, SLOT( keyPressHandler() ) );
    connect ( ui->Buttonn, SIGNAL( pressed() ), this, SLOT( keyPressHandler() ) );
    connect ( ui->Buttonm, SIGNAL( pressed() ), this, SLOT( keyPressHandler() ) );

    connect ( ui->Button0, SIGNAL( pressed() ), this, SLOT( keyPressHandler() ) );
    connect ( ui->Button1, SIGNAL( pressed() ), this, SLOT( keyPressHandler() ) );
    connect ( ui->Button2, SIGNAL( pressed() ), this, SLOT( keyPressHandler() ) );
    connect ( ui->Button3, SIGNAL( pressed() ), this, SLOT( keyPressHandler() ) );
    connect ( ui->Button4, SIGNAL( pressed() ), this, SLOT( keyPressHandler() ) );
    connect ( ui->Button5, SIGNAL( pressed() ), this, SLOT( keyPressHandler() ) );
    connect ( ui->Button6, SIGNAL( pressed() ), this, SLOT( keyPressHandler() ) );
    connect ( ui->Button7, SIGNAL( pressed() ), this, SLOT( keyPressHandler() ) );
    connect ( ui->Button8, SIGNAL( pressed() ), this, SLOT( keyPressHandler() ) );
    connect ( ui->Button9, SIGNAL( pressed() ), this, SLOT( keyPressHandler() ) );
    connect ( ui->space, SIGNAL( pressed() ), this, SLOT( keyPressHandler() ) );


    connect ( ui->Buttonq, SIGNAL( released() ), this, SLOT( keyReleaseHandler() ) );
    connect ( ui->Buttonw, SIGNAL( released() ), this, SLOT( keyReleaseHandler() ) );
    connect ( ui->Buttone, SIGNAL( released() ), this, SLOT( keyReleaseHandler() ) );
    connect ( ui->Buttonr, SIGNAL( released() ), this, SLOT( keyReleaseHandler() ) );
    connect ( ui->Buttont, SIGNAL( released() ), this, SLOT( keyReleaseHandler() ) );
    connect ( ui->Buttony, SIGNAL( released() ), this, SLOT( keyReleaseHandler() ) );
    connect ( ui->Buttonu, SIGNAL( released() ), this, SLOT( keyReleaseHandler() ) );
    connect ( ui->Buttoni, SIGNAL( released() ), this, SLOT( keyReleaseHandler() ) );
    connect ( ui->Buttono, SIGNAL( released() ), this, SLOT( keyReleaseHandler() ) );
    connect ( ui->Buttonp, SIGNAL( released() ), this, SLOT( keyReleaseHandler() ) );

    connect ( ui->Buttona, SIGNAL( released() ), this, SLOT( keyReleaseHandler() ) );
    connect ( ui->Buttons, SIGNAL( released() ), this, SLOT( keyReleaseHandler() ) );
    connect ( ui->Buttond, SIGNAL( released() ), this, SLOT( keyReleaseHandler() ) );
    connect ( ui->Buttonf, SIGNAL( released() ), this, SLOT( keyReleaseHandler() ) );
    connect ( ui->Buttong, SIGNAL( released() ), this, SLOT( keyReleaseHandler() ) );
    connect ( ui->Buttonh, SIGNAL( released() ), this, SLOT( keyReleaseHandler() ) );
    connect ( ui->Buttonj, SIGNAL( released() ), this, SLOT( keyReleaseHandler() ) );
    connect ( ui->Buttonk, SIGNAL( released() ), this, SLOT( keyReleaseHandler() ) );
    connect ( ui->Buttonl, SIGNAL( released() ), this, SLOT( keyReleaseHandler() ) );

    connect ( ui->Buttonz, SIGNAL( released() ), this, SLOT( keyReleaseHandler() ) );
    connect ( ui->Buttonx, SIGNAL( released() ), this, SLOT( keyReleaseHandler() ) );
    connect ( ui->Buttonc, SIGNAL( released() ), this, SLOT( keyReleaseHandler() ) );
    connect ( ui->Buttonv, SIGNAL( released() ), this, SLOT( keyReleaseHandler() ) );
    connect ( ui->Buttonb, SIGNAL( released() ), this, SLOT( keyReleaseHandler() ) );
    connect ( ui->Buttonn, SIGNAL( released() ), this, SLOT( keyReleaseHandler() ) );
    connect ( ui->Buttonm, SIGNAL( released() ), this, SLOT( keyReleaseHandler() ) );

    connect ( ui->Button0, SIGNAL( released() ), this, SLOT( keyReleaseHandler() ) );
    connect ( ui->Button1, SIGNAL( released() ), this, SLOT( keyReleaseHandler() ) );
    connect ( ui->Button2, SIGNAL( released() ), this, SLOT( keyReleaseHandler() ) );
    connect ( ui->Button3, SIGNAL( released() ), this, SLOT( keyReleaseHandler() ) );
    connect ( ui->Button4, SIGNAL( released() ), this, SLOT( keyReleaseHandler() ) );
    connect ( ui->Button5, SIGNAL( released() ), this, SLOT( keyReleaseHandler() ) );
    connect ( ui->Button6, SIGNAL( released() ), this, SLOT( keyReleaseHandler() ) );
    connect ( ui->Button7, SIGNAL( released() ), this, SLOT( keyReleaseHandler() ) );
    connect ( ui->Button8, SIGNAL( released() ), this, SLOT( keyReleaseHandler() ) );
    connect ( ui->Button9, SIGNAL( released() ), this, SLOT( keyReleaseHandler() ) );

    connect ( ui->space, SIGNAL( released() ), this, SLOT( keyReleaseHandler() ) );

    outputText = "";
    shift = false;
    ui->lineEdit->setFocus();
}

KeyboardDialog::~KeyboardDialog()
{
    delete ui;
}

void KeyboardDialog::setText(QString text)
{
    ui->lineEdit->setText(text);
}

void KeyboardDialog::keyPressHandler()
{
    QPushButton *button = (QPushButton *)sender();
    button->setStyleSheet(QString("QPushButton{background-color: rgb(14, 187, 255); color: rgb(243, 243, 243);}"));

    QString inputText = button->text();

    if (inputText == "Space")
    {
        outputText += " ";
    }
    else if(inputText == "&&")
    {
        outputText += "&";
    }
    else if(inputText == "\\")
    {
        outputText += ui->Buttona->text() ;
    }
    else
    {
        if(shift)
        {
            shift = false;
            outputText += inputText.toUpper();
        }
        else
        {
            outputText += inputText;
        }
    }


    ui->lineEdit->setText(outputText);
}

void KeyboardDialog::keyReleaseHandler()
{
    QPushButton *button = (QPushButton *)sender();
    button->setStyleSheet(QString("QPushButton{background-color: rgb(46, 52, 54); color: rgb(243, 243, 243);}"));
}

void KeyboardDialog::keyboardHandler()
{
    QPushButton *button = (QPushButton *)sender();

    QString inputText = button->text();

    if (inputText == "Space")
    {
        outputText += " ";
    }
    else if(inputText == "&&")
    {
        outputText += "&";
    }
    else if(inputText == "\\")
    {
        outputText += ui->Buttona->text() ;
    }
    else
    {
        if(shift)
        {
            shift = false;
            outputText += inputText.toUpper();
        }
        else
        {
            outputText += inputText;
        }
    }


    ui->lineEdit->setText(outputText);

}

void KeyboardDialog::clearLineEdit()
{
    outputText="";
    ui->lineEdit->setText(outputText);
}

void KeyboardDialog::on_shift_clicked()
{
    shift = true;
}


void KeyboardDialog::on_char_2_toggled(bool checked)
{
    if(checked)
    {
        ui->Button0->setText("`");
        ui->Button1->setText("~");
        ui->Button2->setText("!");
        ui->Button3->setText("@");
        ui->Button4->setText("#");
        ui->Button5->setText("$");
        ui->Button6->setText("%");
        ui->Button7->setText("^");
        ui->Button8->setText("&&");// trUtf8("\u0040")
        ui->Button9->setText("*");

        ui->Buttonq->setText("(");
        ui->Buttonw->setText(")");
        ui->Buttone->setText("-");
        ui->Buttonr->setText("_");
        ui->Buttont->setText("=");
        ui->Buttony->setText("+");
        ui->Buttonu->setText("[");
        ui->Buttoni->setText("]");
        ui->Buttono->setText("{");
        ui->Buttonp->setText("}");

        ui->Buttona->setText(trUtf8("\\"));
        ui->Buttons->setText("|");
        ui->Buttond->setText(";");
        ui->Buttonf->setText(":");
        ui->Buttong->setText("'");
        ui->Buttonh->setText("\"");
        ui->Buttonj->setText("/");
        ui->Buttonk->setText("?");
        ui->Buttonl->setText("");

        ui->Buttonz->setText("<");
        ui->Buttonx->setText(">");
        ui->Buttonc->setText(",");
        ui->Buttonv->setText(".");
        ui->Buttonb->setText("");
        ui->Buttonn->setText("");
        ui->Buttonm->setText("");

    }
    else
    {
        ui->Button0->setText("0");
        ui->Button1->setText("1");
        ui->Button2->setText("2");
        ui->Button3->setText("3");
        ui->Button4->setText("4");
        ui->Button5->setText("5");
        ui->Button6->setText("6");
        ui->Button7->setText("7");
        ui->Button8->setText("8");
        ui->Button9->setText("9");

        ui->Buttonq->setText("q");
        ui->Buttonw->setText("w");
        ui->Buttone->setText("e");
        ui->Buttonr->setText("r");
        ui->Buttont->setText("t");
        ui->Buttony->setText("y");
        ui->Buttonu->setText("u");
        ui->Buttoni->setText("i");
        ui->Buttono->setText("o");
        ui->Buttonp->setText("p");

        ui->Buttona->setText("a");
        ui->Buttons->setText("s");
        ui->Buttond->setText("d");
        ui->Buttonf->setText("f");
        ui->Buttong->setText("g");
        ui->Buttonh->setText("h");
        ui->Buttonj->setText("j");
        ui->Buttonk->setText("k");
        ui->Buttonl->setText("l");

        ui->Buttonz->setText("z");
        ui->Buttonx->setText("x");
        ui->Buttonc->setText("c");
        ui->Buttonv->setText("v");
        ui->Buttonb->setText("b");
        ui->Buttonn->setText("n");
        ui->Buttonm->setText("m");
    }
}

void KeyboardDialog::on_clear_clicked()
{
    outputText="";
    ui->lineEdit->setText(outputText);
}

void KeyboardDialog::on_enterButton_clicked()
{
    emit entered(outputText);
    outputText="";
    ui->lineEdit->setText(outputText);
    this->close();
}

void KeyboardDialog::on_backButton_clicked()
{
    outputText.remove(outputText.length()-1,outputText.length());
    ui->lineEdit->setText(outputText);
}
