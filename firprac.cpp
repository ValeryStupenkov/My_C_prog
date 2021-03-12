#include <iostream>
#include <cstring>
#include <fstream>
using namespace std;

const int count=10;

class SubjPointer;

class Element{     // предметный указатель
    int index;      // номер слова в списке 
    int num[count]; // номера страниц 
    char word[50];  
    Element *next;
    Element *prev;
public:
    Element(){index=0; next=prev=NULL;}
    friend class SubjPointer;
    friend ostream &operator<<(ostream &str, SubjPointer &ls);
};

class SubjPointer{        //список предметных указателей
    Element *head; // указатель на начало списка
    Element *tail; // указатель на конец списка
    int n;             // количество элементов в списке
public:
    SubjPointer(){ head=tail=NULL; n=0;};  // конструктор 
    ~SubjPointer(){                        // деструктор
        Element *tmp=NULL;
        while (head!=NULL){
            tmp=head->next;
            delete head;
            head=tmp;
        }
        n=0;
    };
    void addsubj(const char *word, const int *pages){    // добавление элемента в список
        int id=0;
        Element *tmp=new Element();
        if (n!=0){       // если в списке есть элементы
            id=tail->index+1;
            tmp->prev=tail;
            tail->next=tmp;
            tail=tmp;    
        }
        else {
            tmp->prev-NULL;
            head=tail=tmp;
        }
        strcpy(tmp->word,word);
        for (int i=0; i<count;i++)
            tmp->num[i]=pages[i];
        tmp->index=id;
        tmp->next=NULL;
        n++;
    }

    Element *getsubj(const int id){   // посик элемента в списке по индексу
        if (id>n || id<0){
            cerr<<"Error!"<<endl;
            return NULL;
        }    
        Element *tmp=head;
        while (tmp!=NULL){
            if (id==tmp->index)
                return tmp;
            else
                tmp=tmp->next;
        }
        return NULL;
    }
    
    void delsubj(const int id){    // удаление элемента из списка
        Element *tmp=NULL;
        Element *tmp2=NULL;
        int newids;
        if ((tmp=getsubj(id))!=NULL){
            if (tmp->prev!=NULL){
                tmp2=tmp->prev;
                tmp2->next=tmp->next;
            }
            else {
                head=tmp->next;
            }
            if (tmp->next!=NULL) {
                tmp2=tmp->next;
                tmp2->prev=tmp->prev;
            } 
            else
                tail=tmp->prev;
            delete tmp;
            while (tmp2!=NULL){
                tmp2->index--;
                tmp2=tmp2->next;
            }  
        }
        n--;   
        cout<<"Выполнено успешно!"<<endl;
    }
        
    Element *findsubj(const char *word){    // поиск элемента в списке по слову
        if (word==NULL)
            return NULL;
        Element *tmp=head;
        while (tmp!=NULL){
            if (!strcmp(word,tmp->word))
                return tmp;
            tmp=tmp->next;
        }
        return NULL;
    }

    int getlength(){ return n;}    // получение длины списка
    
    friend ostream &operator<<(ostream &str, SubjPointer &ls){   // вывод на экран всех элементов списка
        Element *tmp=NULL;
        str<<"\n";
        for (int i=0;i<ls.getlength();i++){
            tmp=ls.getsubj(i);
            str<<tmp->word<<": ";
            for (int i=0;i<count;i++){
                if (tmp->num[i]>0)
                    str<<tmp->num[i]<<" ";
            }
            str<<endl;
        }
        str<<endl;
        return str; 
    }

    void loadfile(const char* filename){    // ввод указателя из файла
        char word[50];
        int i,tmp=0,num[count];
        fstream fd;   
        fd.open(filename,ios::in);
        if (!fd){
            cout<<"\nError loading\n";
            return;
        }
        while (fd.good()){
            word[0]='\0';
            for (i=0;i<count;i++)
                num[i]=0;
            fd>>word;
            for(i=0;i<count;i++)
                fd>>num[i];
                addsubj(word,num);
        }
        fd.close();
        cout<<"Выполнено успешно!"<<endl;
    }
    
    int *getnum(const char *word){   // получение номера слова в списке
        Element *tmp=NULL;
        if ((tmp=findsubj(word))==NULL){
            cerr<<"Этого слова в списке нет"<<endl;
            return NULL;
        }
        return tmp->num;    
    }

    void printnum(const char *word){   // печать номеров страниц
        if (word==NULL){
            cerr<<"Error--empty word"<<endl;
            exit;
        }
        int *tmp=NULL;
        if ((tmp=getnum(word))==NULL){
            cerr<<"Ошибка, слово не найдено"<<endl;
            exit;
        }
        cout<<word<<": ";
        for (int i=0;i<count;i++){
            if (tmp[i]>0)
                cout<<tmp[i]<<" ";
        }
        cout<<"\n";
    }

    void entersubj(){     // ввод указателя с клавиатуры
        char word[50],yn='y';
        int i,c=0,num[count];
        while (yn=='y'){
            for (i=0;i<count;i++)
                num[i]=0;  
            cout<<"\nВведите слово: ";
            cin>>word;
            cout<<"\nВведите количество страниц: ";
            cin>>c;
            cout<<"\nВведите номера страниц: ";
            for (i=0;i<c;i++){
                cin>>num[i];
            }
            addsubj(word,num);
            cout<<"\nСлово успешно добавлено"<<endl;
            cout<<"Продолжить ввод?-y/n ";
            cin>>yn;              
       }
    } 
};

int main(){
    SubjPointer l;
    string a;
    int id,switcher_a;
    int *pointid;
    char filename[20];
    char word[50];
    cout<<"Приветствую! Выберите нужный вам вариант из списка: "<<endl;
    while(1){
        cout<<" 1 -- ввести указатель с клавитауры"<<endl;
        cout<<" 2 -- ввести указатель из файла"<<endl;
        cout<<" 3 -- вывести список указателей"<<endl;
        cout<<" 4 -- вывести номера страниц для указанного слова"<<endl;
        cout<<" 5 -- удалить элемент из указателя по индексу"<<endl;
        cout<<" 6 -- удалить элемент из указателя по слову"<<endl;
        cout<<" exit-- выход"<<endl;
        cin>>a;
        if (a=="exit"){
            return 0;
        }
        switcher_a=stoi(a);
        switch (switcher_a){
            case 1:
            {
                l.entersubj();
                break;
            }
            case 2:
            {
                cout<<"Введите имя файла: ";
                cin>>filename;
                l.loadfile(filename);
                break;
            }
            case 3:
            {
                cout<<l<<endl;
                break;
            }    
            case 4:
            {
                cout<<"Введите слово: ";
                cin>>word;
                l.printnum(word);
                break;        
            }
            case 5:
            {
                cout<<"Введите номер элемента в списке: ";
                cin>>id;
                l.delsubj(id);
                break;
            }
            case 6:
            {
                cout<<"Введите слово: ";
                cin>>word;
                pointid=l.getnum(word);
                if (pointid!=NULL)
                    l.delsubj(*pointid);
                break;
            }
        }
    }
    return 0;
}
