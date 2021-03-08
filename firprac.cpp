#include <iostream>
#include <cstring>
#include <fstream>
using namespace std;

const int count=10;

class SubjPointer{
public:
    int index;
    int num[count];
    char word[50];
    SubjPointer *next;
    SubjPointer *prev;
    SubjPointer(){index=0; next=prev=NULL;};
    SubjPointer(char* w){strcpy(word,w); next=prev=NULL;};
    SubjPointer(const SubjPointer &s){
        strcpy(word,s.word);
        for (int i=0; i<count; i++) 
            num[i]=s.num[i];
    };
};

class List{
    SubjPointer *head; // указатель на начало списка
    SubjPointer *tail; // указатель на конец списка
    int n;             // количество элементов в списке
public:
    List(){ head=tail=NULL; n=0;};  // конструктор 
    ~List(){                        // деструктор
        SubjPointer *tmp=NULL;
        while (head!=NULL){
            tmp=head->next;
            delete head;
            head=tmp;
        }
        n=0;
    };
    void addsubj(const char * const word, const int * const pages){    // добавление элемента в список
        int id=0;
        SubjPointer *tmp=new SubjPointer();
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

    SubjPointer *getsubj(const int id){   // посик элемента в списке по индексу
        if (id>n || id<0){
            cerr<<"Error!"<<endl;
            return NULL;
        }    
        SubjPointer *tmp=head;
        while (tmp!=NULL){
            if (id==tmp->index)
                return tmp;
            else
                tmp=tmp->next;
        }
        return NULL;
    }
    
    void delsubj(const int id){    // удаление элемента из списка
        SubjPointer *tmp=NULL;
        SubjPointer *tmp2=NULL;
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
    }
        
    SubjPointer *findsubj(const char *word){    // поиск элемента в списке по слову
        if (word==NULL)
            return NULL;
        SubjPointer *tmp=head;
        while (tmp!=NULL){
            if (!strcmp(word,tmp->word))
                return tmp;
            tmp=tmp->next;
        }
        return NULL;
    }

    int getlength(){ return n;}
    
    friend ostream &operator<<(ostream &str, List &ls){   // вывод на экран всех элементов списка
        SubjPointer *tmp=NULL;
        str<<"\n";
        for (int i=0;i<ls.getlength();i++){
            tmp=ls.getsubj(i);
            str<<tmp->word<<": ";
            for (int i=0;i<count;i++){
                if (tmp->num[i]>0)
                    str<<tmp->num[i]<<" ";
                str<<endl;
            }
        }
        str<<endl;
        return str; 
    }

    void loadfile(const char* filename){
        char word[50];
        int i,tmp=0,num[count];
        fstream fd;   
        fd.open(filename,ios::in);
        if (!fd){
            cout<<"\nError loading\n";
            exit;
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
    }
    
    int *getnum(const char *word){
        SubjPointer *tmp=NULL;
        if ((tmp=findsubj(word))==NULL){
            cerr<<"Этого слова в списке нет"<<endl;
            return NULL;
        }
        return tmp->num;    
    }

    void printnum(const char *word){
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
            if (tmp[i]>0);
                cout<<tmp[i]<<" ";
        }
        cout<<"\n";
    }

    void entersubj(){
        char word[50];
        int i,c=0,num[count];
        for (i=0;i<count;i++)
            num[i]=0;  
        cout<<"Введите слово: ";
        cin>>word;
        cout<<"\nВведите количество страниц: ";
        cin>>c;
        cout<<"\nВведите номера страниц: ";
        for (i=0;i<c;i++){
            cin>>num[i];
        }
        addsubj(word,num);
        cout<<"\nСлово успешно добавлено"<<endl;
    } 
};

int main(){
    List l;
    string a;
    int i,b;
    char filename[20];
    char word[50];
    
    while(1){
        cout<<"Приветствую! Выберите нужный вам вариант из списка: "<<endl;
        cout<<" 1 -- ввести указатель с клавитауры"<<endl;
        cout<<" 2 -- ввести указатель из файла"<<endl;
        cout<<" 3 -- вывести список указателей"<<endl;
        cout<<" 4 -- вывести номера страниц для указанного слова"<<endl;
        cout<<" 5 -- удалить элемент из указателя"<<endl;
        cout<<" exit-- выход"<<endl;
        cin>>a;
        if (a=="exit"){
            return 0;
        }
        b=stoi(a);
        switch (b){
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
                cin>>i;
                l.delsubj(i);
                break;
            }
        }
    }
    return 0;
}
