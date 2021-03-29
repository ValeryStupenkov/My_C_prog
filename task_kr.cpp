#include <iostream>

using namespace std;

class FloatPair{
    float a,b;
public:
    FloatPair(float x, float y){ a=x; b=y;}
    virtual float Measure() const = 0;
    const float geta() const { return a;}
    const float getb() const {return b;}
};

class BadSegment{};
class BadAddition;

class Segment : public FloatPair{
public: 
    Segment(float x, float y) : FloatPair(x,y){
        BadSegment s;
        if (x>y)
            throw s;    
    }
    float Measure() const {
        return getb()-geta();
    }
    Segment operator+(const Segment &x);
};

class BadAddition{
    float A1,B1,A2,B2;
public:
    BadAddition(const Segment &x, const Segment &y){
        A1=x.geta();
        B1=x.getb();
        A2=y.geta();
        B2=y.getb();   
    }
    float GetA1() const {return A1;}
    float GetA2() const {return A2;}
    float GetB1() const {return B1;}
    float GetB2() const {return B2;}
};

Segment Segment::operator+(const Segment &x){
        float tmp1,tmp2;
        if (x.geta()>getb()||x.getb()<geta()){
            BadAddition s(*this,x);
            throw s;
        }
        if (x.geta()<geta())
            tmp1=x.geta();   
        else 
            tmp1=geta();
        if (x.getb()>getb())
            tmp2=x.getb();
        else 
            tmp2=getb();
        Segment tmp(tmp1,tmp2); 
        return tmp;
    }

int main(){
    try { 

             Segment f(1,2), g(0.5, 5), h(2.5, 6.5);

             printf("%3.3f, %3.3f, %3.3f\n", (f+g). Measure(), (g+h).Measure(), (f+g+h).Measure());

             printf("%3.3f \n", (f+h).Measure()); 

     }
     catch (const BadAddition &bad) { printf("Bad addition: [%3.3f; %3.3f] + [%3.3f; %3.3f]\n", bad.GetA1(), bad.GetB1(), bad.GetA2(), bad.GetB2()); }

     catch (BadSegment b) { printf("Bad segment\n"); }

     return 0; 
 
}
