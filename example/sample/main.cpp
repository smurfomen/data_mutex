#include <QCoreApplication>
#include <thread>
#include <QThread>
#include <qdatamutex.h>
#include <QDebug>
#include <sample_classes.h>


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QDataMutex<A> A_storage(A(0,0));

    // запустить отдельный поток - если все верно - выполнение продолжится после завершения потока
    std::thread * t = new std::thread([&](){
        auto box = A_storage.lock();
        auto data = box.data();
        qDebug()<<data->a << ++data->b;
        QThread::sleep(1);
    });

    // задержка нужна что бы поток успел захватить владение над данными чтобы продемонстрировать, что код блокирующий
    QThread::msleep(100);

    // выполнить функцию блокированно - если все верно - выполнение будет ожидать уничтожения box в потоке выше
    A_storage.locked([](A*obj){
        qDebug()<<obj->a << ++obj->b;
        QThread::sleep(1);
    });

    {
        auto box = A_storage.lock();
        auto data = box.data();

        qDebug()<<data->a << ++data->b;
    }

    {
        auto box = A_storage.lock();
        auto data = box.data();
        qDebug()<<data->a << ++data->b;
    }

    A_storage = A(2,4);

    {
        auto box = A_storage.lock();
        auto data = box.data();
        qDebug()<<"DataMutex::lock()"<<data->a << ++data->b;
    }

    t->join();






    QDataMutex<B> ab(B(1,0, "Первый"));

    ab.locked([](B*obj){
        qDebug()<< obj->a << ++obj->b << obj->m;
        QThread::sleep(1);
    });

    std::thread * t1 = new std::thread([&](){
        auto box = ab.lock();
        auto data = box.data();
        data ->m= "Второй";
        qDebug()<<data->a << ++data->b << data->m;
        QThread::sleep(1);
    });

    QThread::msleep(100);

    {
        auto box = ab.lock();
        auto data = box.data();
        data->m = "Третий";
        qDebug()<<data->a << ++data->b << data->m;
    }

    {
        auto box = ab.lock();
        auto data = box.data();
        data->m = "Четвертый";
        qDebug()<<data->a << ++data->b << data->m;
    }

    t1->join();




    auto o = Some(A(222, 333));

    o.if_none([](){
        qDebug()<<"wrong";
    }).if_some([](A & a){
        qDebug()<<"first";
        qDebug()<<a.a++<<a.b++;
    })
    .if_some([](A & a) {
        qDebug()<<"second";
        qDebug()<<a.a<<a.b;
    });


    qDebug()<<o.unwrap().a;






    return a.exec();
}
