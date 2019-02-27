#include "keyboardform.h"
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QFontDatabase>
#include <QApplication>
#include <QDebug>


const char *kbNumber = "1234567890";
const char *kbLetter = "qwertyuiopasdfghjklzxcvbnm";
const char *kbSymbol = "!@#$%^&*()-_+=[]{}|\\:;\"<>?";
const int verSpace = 10;
const int horSpace = 10;
const int margins = 10;
const int mainWidth = 1295;
const int mainHeight = 411;
const int btnHeight = (mainHeight - verSpace*5)/5;
const int btnWidth = (mainWidth - margins*2 - horSpace*10)/10.8+0.5;

const int hanziCandidate = 7;
const int letter1 = 10;
const int letter2 = 9;
const int letter3 = 7;

KeyboardForm::KeyboardForm(QWidget *parent) : QDialog(parent),
    m_inputMode(ImEn),m_shiftKey(false),m_hanziPageCnt(0),
    m_curHanziPage(0)
{
    m_listCharsBtns.clear();
    m_cachePinYin.clear();

    setWindowModality(Qt::WindowModal);
    setWindowFlags(windowFlags() | Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::WindowDoesNotAcceptFocus);

    //图标字体
    int fontId = QFontDatabase::addApplicationFont(":/font/FontAwesome.otf");
    QString fontName = QFontDatabase::applicationFontFamilies(fontId).at(0);
    QFont icofont(fontName);
    icofont.setPixelSize(28);

    //按键字体
    QFont deffont("Courier");
    deffont.setPixelSize(24);

    // 第一行 显示CN
    m_labPyText = new QLabel;
    m_labPyText->setFixedHeight(30);
    //m_labPyText->setStyleSheet("background: #a0ffffff; border-color: gray; border-width: 1px; border-style: solid; border-radius: 5px;");
    QHBoxLayout *h1 = new QHBoxLayout;
    h1->addWidget(m_labPyText);
    h1->addStretch();

    // 第二行 显示汉字，八个候选项
    QHBoxLayout *h2 = new QHBoxLayout;
    h2->setSpacing(1);
    for (int i = 0; i < hanziCandidate; i++)
    {
        QPushButton *btn = new QPushButton;
        btn->setFont(deffont);
        btn->setStyleSheet("QPushButton{border: none; border-image: none; font-size: 18px}");
        h2->addWidget(btn);
        m_listHanzi.append(btn);
        if (i == 0 || i == hanziCandidate-1)
        {
            btn->setEnabled(false);
            btn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            btn->setText(i == 0 ? "\xC2\xAB" : "\xC2\xBB");
        }
        else
        {
            connect(btn, SIGNAL(clicked()), SLOT(hanziClicked()));
        }
    }
    connect(m_listHanzi[0], SIGNAL(clicked()), SLOT(prevPage()));
    connect(m_listHanzi[hanziCandidate-1], SIGNAL(clicked()), SLOT(nextPage()));

    // CN+汉子 整个放入pyFrm Widget
    m_pyFrm = new QWidget;
    m_pyFrm->setLayout(h2);
    //m_pyFrm->setStyleSheet("background:#FFFFFF");
    //m_pyFrm->setStyleSheet(QString("QWidget{background:qlineargradient(spread:pad,x1:0,y1:0,x2:0,y2:1,stop:0 %1,stop:1 %2);}").arg("#D8D9DE").arg("#C8C8D0"));
    QVBoxLayout *pyLayout = new QVBoxLayout;
    pyLayout->addLayout(h1);
    pyLayout->addWidget(m_pyFrm);

    // 第三行 数字
    QHBoxLayout *h3 = new QHBoxLayout;

    // 波浪号~
    QPushButton *btnWave = new QPushButton("~");
    btnWave->setFont(deffont);
    btnWave->setFixedSize(btnWidth, btnHeight);
    connect(btnWave, SIGNAL(clicked()), SLOT(numClicked()));//直接输出字符

    h3->addWidget(btnWave);

    for (int j = 0; j < 10; j++)
    {
        QPushButton *btn = new QPushButton(QString(QLatin1Char(kbNumber[j])));
        btn->setFixedSize(btnWidth, btnHeight);
        btn->setFont(deffont);
        connect(btn, SIGNAL(clicked()), SLOT(numClicked()));
        h3->addWidget(btn);
    }

    // 第一行字母

    QHBoxLayout *h4 = new QHBoxLayout;
    for (int j = 0; j < letter1; j++)
    {
        QPushButton *btn = new QPushButton(QString(QLatin1Char(kbLetter[j])));
        btn->setFixedSize(btnWidth, btnHeight);
        btn->setFont(deffont);
        connect(btn, SIGNAL(clicked()), SLOT(letterClicked()));
        h4->addWidget(btn);
        m_listCharsBtns.append(btn);
    }

    QPushButton *btnBack = new QPushButton();
    btnBack->setFont(icofont);
    btnBack->setText(QChar(0xf060));
    btnBack->setFixedSize(btnWidth, btnHeight);
    btnBack->setAutoRepeat(true);
    connect(btnBack, SIGNAL(clicked()), SLOT(backSpace()));
    h4->addWidget(btnBack);

    // 第二行字母

    QHBoxLayout *h5 = new QHBoxLayout;
    h5->addStretch();
    for (int j = 0; j < letter2; j++)
    {
        QPushButton *btn = new QPushButton(QString(QLatin1Char(kbLetter[letter1+j])));
        btn->setFont(deffont);
        btn->setFixedSize(btnWidth, btnHeight);
        connect(btn, SIGNAL(clicked()), SLOT(letterClicked()));
        h5->addWidget(btn);
        m_listCharsBtns.append(btn);
    }

    QPushButton *btnEnter = new QPushButton("ENTER");
    btnEnter->setFont(deffont);
    btnEnter->setFixedSize(btnWidth*1.5, btnHeight);
    connect(btnEnter, SIGNAL(clicked()), SLOT(enter()));
    h5->addWidget(btnEnter);
    h5->addStretch();

    // 第三行字母

    QHBoxLayout *h6 = new QHBoxLayout;
    m_btnShift = new QPushButton(QChar(0xf062));
    m_btnShift->setFont(icofont);
    m_btnShift->setFixedSize(btnWidth, btnHeight);
    connect(m_btnShift, SIGNAL(clicked()), SLOT(shiftClicked()));
    h6->addWidget(m_btnShift);
    for (int j = 0; j < letter3; j++)
    {
        QPushButton *btn = new QPushButton(QString(QLatin1Char(kbLetter[letter1 + letter2 + j])));
        btn->setFont(deffont);
        btn->setFixedSize(btnWidth, btnHeight);
        connect(btn, SIGNAL(clicked()), SLOT(letterClicked()));
        h6->addWidget(btn);
        m_listCharsBtns.append(btn);
    }

    QString letter(",./");
    for (int j = 0; j < 3; j++)
    {
        QPushButton* btn = new QPushButton(letter.at(j));
        btn->setFont(deffont);
        btn->setFixedSize(btnWidth, btnHeight);
        connect(btn, SIGNAL(clicked()), SLOT(numClicked()));
        h6->addWidget(btn);
    }

    // 第四行

    QHBoxLayout *h7 = new QHBoxLayout;

    m_btnSymbol = new QPushButton(".?123");
    m_btnSymbol->setFont(deffont);
    m_btnSymbol->setFixedSize(btnWidth*1.5, btnHeight);
    connect(m_btnSymbol, SIGNAL(clicked()), SLOT(changeSymbol()));
    h7->addWidget(m_btnSymbol);

    m_btnChange = new QPushButton(QChar(0xf0ac));
    m_btnChange->setFont(icofont);
    m_btnChange->setProperty("Mode", ImEn);
    m_btnChange->setFixedSize(btnWidth, btnHeight);
    connect(m_btnChange, SIGNAL(clicked()), SLOT(changeInputMode()));
    h7->addWidget(m_btnChange);

    m_btnSpace = new QPushButton("EN");  // 空格
    m_btnSpace->setFont(deffont);
    m_btnSpace->setFixedHeight(btnHeight);
    connect(m_btnSpace, SIGNAL(clicked()), SLOT(space()));
    h7->addWidget(m_btnSpace);

    QPushButton *btnSplit = new QPushButton("'");
    btnSplit->setFont(deffont);
    btnSplit->setFixedSize(btnWidth, btnHeight);
    connect(btnSplit, SIGNAL(clicked()), SLOT(letterClicked()));
    h7->addWidget(btnSplit);

    QString text = QChar(0xf11c);
    text.append(QChar(0xf103));
    QPushButton *btnHide = new QPushButton(text);
    btnHide->setFont(icofont);
    btnHide->setFixedSize(btnWidth*1.5, btnHeight);
    connect(btnHide, SIGNAL(clicked()), SLOT(hide()));
    h7->addWidget(btnHide);

    QVBoxLayout *boradLayout = new QVBoxLayout;
    boradLayout->setMargin(margins);
    boradLayout->setSpacing(margins);

    boradLayout->addLayout(h3);
    boradLayout->addLayout(h4);
    boradLayout->addLayout(h5);
    boradLayout->addLayout(h6);
    boradLayout->addLayout(h7);

    // 键盘widget容器
    m_btnFrm = new QWidget();
    m_btnFrm->setLayout(boradLayout);
    m_pyFrm->setFixedHeight(90);

    // 主布局，CN、汉字 + 按键
    m_mainLayout = new QVBoxLayout;
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);
    m_mainLayout->addLayout(pyLayout);
    m_mainLayout->addWidget(m_btnFrm);
    setLayout(m_mainLayout);

    connect(&m_pinyin, SIGNAL(pinyinTextChanged(QString)), this, SLOT(setText(QString)));
    //setFixedHeight(m_pyFrm->height() + m_labPyText->height() + m_btnFrm->height());
    changeStyle();
}


void KeyboardForm::letterClicked()
{
    QString text = ((QPushButton*)sender())->text();
    qDebug() << "current input mode" << m_inputMode;
    // if (m_inputMode == ImNum || m_shiftKey || text == "'" || m_inputMode == ImEn)
    if (m_inputMode == ImNum || m_inputMode == ImEn)
    {
        sendKeyToFocusItem(text);
    }
    else
    {
        m_cachePinYin.append(text);
        m_pinyin.Matching(m_cachePinYin, m_inputMode == ImEn);
        if (m_pinyin.HanziModel.size())
        {
            m_hanziPageCnt = m_pinyin.HanziModel.size() / (hanziCandidate-2)
                    + int(m_pinyin.HanziModel.size() % (hanziCandidate-2) > 0);
            m_curHanziPage = 1;
        }
        else
        {
            m_hanziPageCnt = 0;
        }
        displayHanzi();
    }
}

void KeyboardForm::numClicked()
{
    sendKeyToFocusItem(((QPushButton*)sender())->text());
}

void KeyboardForm::hanziClicked()
{
    sendKeyToFocusItem(((QPushButton*)sender())->text());
    m_cachePinYin.clear();
    m_labPyText->clear();
    m_labPyText->hide();
    m_hanziPageCnt = 0;
    displayHanzi();
}

void KeyboardForm::setText(QString str)
{
    m_labPyText->setVisible(str.size());
    m_labPyText->setText(str);
}

void KeyboardForm::shiftClicked()
{
    if (m_shiftKey)
    {
        m_shiftKey = false;
    }
    else
    {
        m_shiftKey = true;
       // m_btnChange->setProperty("Mode", ImEn);
       // m_btnSpace->setText("EN");
    }

    // 中文
    if(m_inputMode == ImCn)
    {
        // 输出已有的字母
        sendKeyToFocusItem(m_cachePinYin);
        // 所有CN相关清空

        m_labPyText->clear();
        m_cachePinYin.clear();
        m_hanziPageCnt = 0;
        displayHanzi();
    }
    else if(m_inputMode == ImEn){
        // 重新布局字母
        for (int i = 0; i < m_listCharsBtns.size(); i++)
        {
            m_listCharsBtns[i]->setText(QString(QLatin1Char(m_shiftKey ? kbLetter[i] - 32 : kbLetter[i])));
        }
    }
}

void KeyboardForm::changeInputMode()
{
    m_btnChange->setProperty("Mode", m_inputMode = m_btnChange->property("Mode").toInt() == ImEn ? ImCn : ImEn);
    m_btnSpace->setText(m_inputMode == ImCn ? "CN" : "EN");

    for (int i = 0; i < m_listCharsBtns.size(); i++)
    {
        m_listCharsBtns[i]->setText(QString(QLatin1Char(kbLetter[i])));
    }

    // 输出已有的字母
    sendKeyToFocusItem(m_cachePinYin);

    m_labPyText->clear();
    m_cachePinYin.clear();
    m_hanziPageCnt = 0;
    displayHanzi();

    m_btnSymbol->setText(".?123");
    m_shiftKey = false;
    //m_btnShift->setStyleSheet("QPushButton{color:#FFFFFF; border-image: url(:/images/btn2.png); border-width: 5;} QPushButton:pressed{border-image: url(:/images/btn3.png); border-width: 5;}");
}

void KeyboardForm::changeSymbol()
{
    if (m_btnSymbol->text() == ".?123")
    {
        m_btnShift->setEnabled(false);
        m_btnSymbol->setText("BACK");

        // 输出已有的字母
        sendKeyToFocusItem(m_cachePinYin);

        m_hanziPageCnt = 0;
        m_labPyText->clear();
        m_cachePinYin.clear();
        displayHanzi();

        m_shiftKey = false;
        //m_btnShift->setStyleSheet("QPushButton{color:#FFFFFF; border-image: url(:/images/btn2.png); border-width: 5;} QPushButton:pressed{border-image: url(:/images/btn3.png); border-width: 5;}");
        m_inputMode = ImNum;
        m_btnSpace->setText("");
        for (int i = 0; i < m_listCharsBtns.size(); i++)
        {
            m_listCharsBtns[i]->setText(kbSymbol[i] == '&' ? "&&" : QString(QLatin1Char(kbSymbol[i])));
        }
    }
    else
    {
        m_btnShift->setEnabled(true);
        m_btnSymbol->setText(".?123");
        m_inputMode = (InputMode)m_btnChange->property("Mode").toInt();
        m_btnSpace->setText(m_inputMode == ImCn ? "CN" : "EN");
        for (int i = 0; i < m_listCharsBtns.size(); i++)
        {
            m_listCharsBtns[i]->setText(QString(QLatin1Char(kbLetter[i])));
        }
    }
}

void KeyboardForm::space()
{
    sendKeyToFocusItem(" ");
}

void KeyboardForm::enter()
{
    if (m_cachePinYin.size())
    {
        sendKeyToFocusItem(m_cachePinYin);
        m_cachePinYin.clear();
        m_labPyText->clear();
        m_hanziPageCnt = 0;
        displayHanzi();
    }
    else
    {
        sendKeyToFocusItem("\n");
    }
}

void KeyboardForm::backSpace()
{
    if (m_inputMode == ImNum || m_shiftKey)
    {
        sendKeyToFocusItem("\x7F");
    }
    else
    {
        if (m_cachePinYin.size() > 1)
        {
            m_cachePinYin.remove(m_cachePinYin.size()-1, 1);
            m_pinyin.Matching(m_cachePinYin, m_inputMode == ImEn);
            if (m_pinyin.HanziModel.size())
            {
                int size = hanziCandidate-2;
                m_hanziPageCnt = m_pinyin.HanziModel.size() / size + int(m_pinyin.HanziModel.size() % size > 0);
                m_curHanziPage = 1;
            }
            else
            {
                m_hanziPageCnt = 0;
            }
            displayHanzi();
        }
        else if(1 == m_cachePinYin.size())
        {
            m_cachePinYin.clear();
            m_labPyText->clear();
            m_hanziPageCnt = 0;
            displayHanzi();
        }
        else
        {
            m_cachePinYin.clear();
            m_labPyText->clear();
            sendKeyToFocusItem("\x7F");
        }
    }
}

void KeyboardForm::displayHanzi()
{
    if (m_hanziPageCnt <= 0)
    {
        m_listHanzi[0]->setEnabled(false);
        m_listHanzi[hanziCandidate-1]->setEnabled(false);
        for (int i = 0; i < hanziCandidate-2; i++)
        {
            m_listHanzi[i+1]->setText("");
            m_listHanzi[i+1]->setEnabled(false);
        }
    }
    else if (m_hanziPageCnt == 1)
    {
        m_listHanzi[0]->setEnabled(false);
        m_listHanzi[hanziCandidate-1]->setEnabled(false);
        int len = m_pinyin.HanziModel.size();
        for (int i = 0; i < len; i++)
        {
            m_listHanzi[i+1]->setText(m_pinyin.HanziModel[i]);
            m_listHanzi[i+1]->setEnabled(true);
        }
        for (int i = len; i < hanziCandidate-2; i++)
        {
            m_listHanzi[i+1]->setText("");
            m_listHanzi[i+1]->setEnabled(false);
        }
    }
    else if (m_curHanziPage == 1)
    {
        m_listHanzi[0]->setEnabled(false);
        m_listHanzi[hanziCandidate-1]->setEnabled(true);
        for (int i = 0; i < hanziCandidate-2; i++)
        {
            m_listHanzi[i+1]->hide();
            m_listHanzi[i+1]->setText(m_pinyin.HanziModel[i]);
            m_listHanzi[i+1]->setEnabled(true);
            m_listHanzi[i+1]->show();
        }
    }
    else if (m_curHanziPage == m_hanziPageCnt)
    {
        m_listHanzi[0]->setEnabled(true);
        m_listHanzi[hanziCandidate-1]->setEnabled(false);
        qDebug() << m_pinyin.HanziModel << m_pinyin.HanziModel.size();
        int len = m_pinyin.HanziModel.size() % (hanziCandidate-2);
        int idx = (m_hanziPageCnt - 1) * (hanziCandidate-2);
        if (len == 0)
        {
            len = hanziCandidate-2;
        }

        for (int i = 0; i < len; i++)
        {
            m_listHanzi[i+1]->setText(m_pinyin.HanziModel[idx + i]);
            m_listHanzi[i+1]->setEnabled(true);
        }
        for (int i = len; i < hanziCandidate-2; i++)
        {
            m_listHanzi[i+1]->setText("");
            m_listHanzi[i+1]->setEnabled(false);
        }
    }
    else
    {
        m_listHanzi[0]->setEnabled(true);
        m_listHanzi[hanziCandidate-1]->setEnabled(true);
        int idx = (m_curHanziPage - 1) * (hanziCandidate-2);
        for (int i = 0; i < hanziCandidate-2; i++)
        {
            m_listHanzi[i+1]->setText(m_pinyin.HanziModel[idx + i]);
            m_listHanzi[i+1]->setEnabled(true);
        }
    }
}

void KeyboardForm::prevPage()
{
    m_curHanziPage--;
    displayHanzi();
}

void KeyboardForm::nextPage()
{
    m_curHanziPage++;
    displayHanzi();
}

void KeyboardForm::changeStyle()
{
    setStyleSheet("QPushButton{border-image: url(:/images/normal.png); border-width: 5; font: normal } QPushButton:pressed{border-image: url(:/images/press.png); border-width: 5;}");
    //m_labPyText->setStyleSheet("background: #a0ffffff; border-color: gray; border-width: 1px; border-style: solid; border-radius: 5px;");
    m_pyFrm->setStyleSheet("background:#FFFFFF");
    m_btnFrm->setStyleSheet("background-color:rgb(147, 160, 173);");
}

void KeyboardForm::mouseMoveEvent(QMouseEvent *e)
{
    if (m_mousePressed && (e->buttons() && Qt::LeftButton)) {
        this->move(e->globalPos() - m_mousePoint);
        e->accept();
    }
}

void KeyboardForm::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        m_mousePressed = true;
        m_mousePoint = e->globalPos() - this->pos();
        e->accept();
    }
}

void KeyboardForm::mouseReleaseEvent(QMouseEvent *)
{
    m_mousePressed = false;
}
