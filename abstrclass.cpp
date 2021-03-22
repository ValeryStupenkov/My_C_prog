#include <iostream>
#include <string>
#include <cstring>

using namespace std;                                
                                            //Абстрактный класс Person и его наследники: классы Student и Professor + подсчёт общего количества
                                            //людей
class Person{     
    static int kol;     // количество людей
    char *first_name;   // имя 
    char *last_name;    // фамилия
    int size_f;         
    int size_l;
    int year;           // год рождения
public:     
    Person(){
        first_name=last_name=NULL;
        size_f=size_l=0;
        year=0;    
        kol++;
    }
    Person(char *fname,char *lname, int y){     //конструктор
        if (fname==NULL){
            first_name=NULL;
            size_f=0;
        }
        else{
            size_f=strlen(fname)+1;
            first_name=new char[size_f];
            strcpy(first_name,fname);
        }
        if (lname==NULL){
            last_name=NULL;
            size_l=0;
        }
        else{
            size_l=strlen(lname)+1;
            last_name=new char[size_l];
            strcpy(last_name,lname);
        }
        year=y;
        kol++;   
    }
    virtual ~Person(){      // деструктор
        delete[]first_name;
        delete[]last_name;;
        kol--;
    }
    virtual void EnterInf()=0;      // чисто виртуальная функция, обеспечивает ввод доп. информации(то есть группы для студентов, предмета для прфессоров)
    virtual void ShowInf()=0;       // чисто виртуальная функция, показывает доп. информацию
    virtual void CopyInf(const Person &p){};    // виртуальная функция, копирует доп. информацию
    Person(const Person &p){        // конструктор копирования
        size_f=p.size_f;
        size_l=p.size_l;
        first_name=new char[size_f];
        strcpy(first_name,p.first_name);
        last_name=new char[size_l];
        strcpy(last_name,p.last_name);
        year=p.year;
        kol++; 
    }
    const Person& operator=(const Person &p){   // перегруженная операция присваивания
        if (first_name!=NULL){
            delete[] first_name;
            first_name=NULL;
        }
        if (last_name!=NULL){
            delete[] last_name;
            last_name=NULL;
        }
        size_f=p.size_f;
        size_l=p.size_l;
        first_name=new char[size_f];
        last_name=new char[size_l];
        strcpy(first_name,p.first_name);
        strcpy(last_name,p.last_name);
        year=p.year;
        CopyInf(p);
        return *this;    
    }
    friend ostream& operator<<(ostream &str,const Person &p){   // прегруженный оператор вывода
        if (p.first_name==NULL||p.last_name==NULL){
            str<<"Неполные данные!!!"<<endl;    
        }
        str<<p.first_name<<" ";
        str<<p.last_name<<" "<<endl;
        str<<"Год рождения: "<<p.year<<endl;
        return str;
    }
    void EnterPerson(){     // ввод информации о человеке
        string str;
        cout<<"Введите имя: ";
        cin>>str;
        if (first_name!=NULL){
            delete[] first_name;
            first_name=NULL;
        }
        if (last_name!=NULL){
            delete[] last_name;
            last_name=NULL;
        }
        size_f=str.length()+1;
        first_name=new char[size_f];
        strcpy(first_name,str.c_str());
        cout<<"Введите фамилию: ";
        cin>>str;
        size_l=str.length()+1;
        last_name=new char[size_l];
        strcpy(last_name,str.c_str());
        cout<<"Введите год рождения: ";
        cin>>year;
        EnterInf();
    }
    void Printall(){
        cout<<*this;
        ShowInf();            
    }
    static int Getkol(){return kol;}    
    static int Addkol(){kol++; return kol;}
    static int Delkol(){kol--; return kol;}
    int GetYear() const { return year;}
    const char *GetFName() const {return first_name;}
    const char *GetLName() const {return last_name;}
      
};

class Student : public Person{  // класс-наследник 
    int group;  // дополнительная информация
public:
    Student() : Person(){group=0;}
    Student(char *fname,char *lname, int y,int g) : Person(fname,lname,y){
        group=g; 
    }
    Student(const Student &s) : Person(s){
        group=s.group;    
    }
    virtual ~Student(){}
    void ShowInf(){
        cout<<"Род деятельности: Студент "<<"группа: "<<group<<endl;       
    }
    void CopyInf(const Student &p){
        group=p.group;
    }
    void EnterInf(){
        cout<<"Введите номер группы: ";
        cin>>group;
        cout<<endl;
    }
};

class Professor : public Person{    // класс-наследник
    int size_s; 
    char *subj;     // дополнительная информация
public:
    Professor() : Person(){ subj=NULL; size_s=0;}
    Professor(char *fname,char *lname, int y, char *s) : Person(fname,lname,y){ 
        size_s=strlen(s)+1;
        subj=new char[size_s];
        strcpy(subj,s);       
    }
    Professor(const Professor &p) : Person(p){
        size_s=strlen(p.subj)+1;
        subj=new char[size_s];
        strcpy(subj,p.subj);
    }
    virtual ~Professor(){
        delete[]subj;
    }
    void ShowInf(){
        cout<<"Род деятельности: Профессор"<<endl;
        cout<<"Ведёт предмет: "<<subj<<endl;    
    }
    void CopyInf(const Professor &p){
        if (subj!=NULL){
            delete[] subj;
            subj=NULL;
        }
        size_s=strlen(p.subj)+1;
        subj=new char[size_s];
        strcpy(subj,p.subj);
    } 
    void EnterInf(){
        string s;
        cout<<"Введите название предмета: ";
        cin>>s;
        if (subj!=NULL){
            delete[] subj;
            subj=NULL;
        }
        size_s=s.length()+1;
        subj=new char[size_s];
        strcpy(subj,s.c_str());
        cout<<endl;
    }   
};

int Person::kol=0;

int main(){
    char yn;
    int a,n,k;
    string s;
    Student s1;
    Professor p1;
    cout<<"Введите тестовые данные:"<<endl;
    s1.EnterPerson();
    p1.EnterPerson();
   
    cout<<"Hello there! Выберите нужный вариант из списка:"<<endl;
    while(1){
        cout<<endl;
        cout<<"1 -- Проверить конструктор копирования"<<endl;
        cout<<"2 -- Проверить операцию присваивания"<<endl;
        cout<<"3 -- Проверить константные методы"<<endl;
        cout<<"4 -- Проверить статические члены класса"<<endl;
        cout<<"5 -- Выход"<<endl;
        cin>>n;
        switch (n){
            case 1:
            {   
                Student s2(s1);
                cout<<"Копировали s1 в s2"<<endl;
                s2.Printall();
                break;
            }
            case 2:
            {   
                Professor p2;
                p2=p1;
                cout<<"Присвоили p1 в p2"<<endl;
                p2.Printall();
                break;
            }
            case 3:
            {   
                cout<<"Получить имя из s1"<<endl;
                cout<<s1.GetFName()<<endl;
                cout<<"Получить фамилию из p1"<<endl;
                cout<<p1.GetLName()<<endl;
                cout<<"Получить год рождения из s1"<<endl;
                cout<<s1.GetYear()<<endl;
                break;
            }
            case 4:
            {
                cout<<"Количество объектов класса Person: "<<Person::Getkol()<<endl;
                break;
            }
            case 5:
            {
                return 0;
            }
            default:
            {
                cout<<"Такого пункта в меню нет("<<endl;
                break;
            }
        }
    }
    return 0;
}
