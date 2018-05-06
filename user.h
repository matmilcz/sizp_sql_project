#ifndef USER_H
#define USER_H

class User
{
    QString login;
    QString email;
    QString name;
    QString surname;
    QString adress;
    int role; // 0 - niezarejestrowany, 1 - zarejestrowany, 2 - admin

public:
    User(QString l = "Niezarejestrowany", QString e = "", QString n = "ADMIN", QString s = "", QString a = "", int r = 0)
        : login(l), email(e), name(n), surname(s), adress(a), role(r) {}
    void ChangeUser(QString l, QString e, QString n, QString s, QString a, int r)
    {
        login = l;
        email = e;
        name = n;
        surname = s;
        adress = a;
        role = r;
    }
    QString getLogin() { return login; }
    QString getEmail() { return email; }
    QString getName() { return name; }
    QString getSurname() { return surname; }
    QString getAdress() { return adress; }
    int getRole() { return role; }

};

#endif // USER_H
