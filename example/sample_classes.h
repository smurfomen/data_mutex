#ifndef SAMPLE_CLASSES_H
#define SAMPLE_CLASSES_H


#include <QDebug>
class A {
public:
    A(int aa, int bb)
        : a(aa), b(bb)
    {

    }

    A(A && o)
        : a(o.a), b(o.b)
    {
        qDebug()<<"A moved";
    }

    A(const A & o)
        : a(o.a), b(o.b)
    {
        qDebug()<<"A copyed";
    }

    A& operator=(const A& o) = default;

int a;
int b;
};


#include <QString>
class B {
public:
    B(int aa, int bb, const char * msg)
        : a(aa), b(bb), m(msg)
    {

    }
int a;
int b;
const char * m;
};


#endif // SAMPLE_CLASSES_H
