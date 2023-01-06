#include<iostream>
#include<cassert>
#include<ctime>
#include<random>
#include<vector>
#include<chrono>
#include<array>
#include<algorithm>
//
// 1. класс необходимо параметризировать                +
// 2. написать к нему итератор                          +
// 3. добавить обработку исключений                     +
// 4. копирующие и перемещающие                         +
//    конструктор копирования и оператор присваивания   +
// 5. операции типа emplace                             + 
// 6. Добавить список инициализации                     + 

using namespace std;

template<typename T>
class KMatrix;
// Итератор к классу матриц

template <typename T>
class KIterMatr
{
public:
    friend class KMatrix<T>;

    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using difference_type = ptrdiff_t;
    using pointer = T*;
    using reference = T&;

    KIterMatr() {}
    KIterMatr(KMatrix<T>* pArr, int n, int m);
    KIterMatr(const KMatrix<T>* pArr, int n, int m);   // TODO 

    inline T& operator *();
    inline T const& operator *() const;


    KIterMatr& operator ++();
    //{

    //    Col++;
    //    Col = Col % pArr->columns;
    //    if (Col == 0) { Row++; }
    //    return *this;
    //}
    KIterMatr const  operator ++(int); // { KIterMatr t(*this); ++* this; return t; }

    bool operator == (KIterMatr const& it2) const; // { return Col == it2.Col && Row == it2.Row; }
    bool operator != (KIterMatr const& it2) const; // { return !(it2 == *this); }

private:
private:
    KMatrix<T>* pArr;
    int Col;
    int Row;
};

// Сам класс матриц
template<typename T>
class KMatrix
{
protected:
    T** arr = nullptr;
    int rows = 0;
    int columns = 0;

    void Erase()
    {
        cout << "Erase\n";
        if (arr != nullptr)
        {
            for (int i = 0; i < columns; i++)
                arr[i] = nullptr;

            arr = nullptr;
        }
    }

    friend class KIterMatr<T>;
    typedef KIterMatr<T> iterator;
    typedef  KIterMatr<const T> const_iterator;
public:
    KMatrix()
    {
        rows = columns = 0;
        arr = nullptr;
    }
    KMatrix(int aRows, int aColumns)
    {
        columns = aColumns;
        rows = aRows;
        arr = new T * [rows];
        for (int i = 0; i < rows; i++)
            arr[i] = new T[columns];
    }
    KMatrix(const KMatrix& Matr)
    {
        std::cout << "Copy constructor has been called\n";
        rows = Matr.rows;
        columns = Matr.columns;

        arr = (T**) new T * [rows];
        for (int i = 0; i < rows; i++)
            arr[i] = (T*) new T[columns];

        for (int i = 0; i < rows; i++)
            for (int j = 0; j < columns; j++)
                arr[i][j] = Matr.arr[i][j];
    }
    KMatrix(KMatrix&& Matr) noexcept {
        std::cout << "Move constructor has been called\n";
        arr = Matr.arr;
        columns = Matr.columns;
        rows = Matr.rows;
        Matr.columns = 0;
        Matr.rows = 0;
        Matr.arr = nullptr;
    }
    KMatrix(std::initializer_list<std::initializer_list<T>> list) {

        columns = list.begin()->size();
        rows = list.size();

        arr = new T * [rows];
        for (int i = 0; i < rows; i++) {
            arr[i] = new T[columns];
            for (int j = 0; j < columns; j++)
            {
                arr[i][j] = (list.begin() + i)->begin()[j];
            }
        }
    }

    ~KMatrix() { Erase(); }
    void Swap(KMatrix& Matr1)
    {
        KMatrix tmp;
        tmp = Matr1;
        Matr1 = *this;
        *this = tmp;
    }
    KMatrix Transpose()
    {
        KMatrix tmp(columns, rows);
        for (int i = 0; i < columns; i++)
            for (int j = 0; j < rows; j++)
                tmp.arr[i][j] = arr[j][i];
        return tmp;
    }

    int Columns()
    {
        return columns;
    }
    int Rows()
    {
        return rows;
    }
    constexpr int Size() { return rows * columns; }

    vector<T> Sq_to_vec() {
        std::vector<T> vec;
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < columns; j++) {
                vec.push_back(arr[i][j]);
            }
        }
        return vec;
    }

    void change_element(int m, int n, int number) {
        if (n < 0 || m < 0 || m >= rows || n >= columns) {
            throw - 1;
        }
        else {
            std::cout << "element was succesfully changed\n";
            arr[m][n] = number;
        }
    }


    template<typename ...Args>
    void change_element_emplace(int m, int n, Args&&... val) {
        if (n < 0 || m < 0 || m >= rows || n >= columns) {
            throw - 1;
        }
        else {
            std::cout << "element was succesfully changed\n";
            new (&arr[m][n]) T(forward<Args>(val)...);
        }
    }

    bool operator == (KMatrix  Matrix1)
    {
        if (columns == Matrix1.columns && rows == Matrix1.rows)
        {
            for (int i = 0; i < rows; i++)
            {
                for (int j = 0; j < columns; j++)
                {
                    if (arr[i][j] == Matrix1.arr[i][j]) continue;
                    else return false;
                }
            }
            return true;
        }
        else return false;
    }
    bool operator != (KMatrix  Matrix1)
    {
        bool b = true;
        if (Matrix1 == *this) {
            return b;
        }
        else {
            b = false;
            return b;
        }
    }

    KMatrix operator + (int number)
    {
        for (int i = 0; i < rows; i++) {
            arr[i][i] += number;
        }
        return *this;
    }
    KMatrix operator + (const KMatrix& Matrix1)
    {
        KMatrix tmp;
        tmp = Matrix1;
        if (columns == tmp.columns && rows == tmp.rows)
        {
            for (int i = 0; i < columns; i++)
            {
                for (int j = 0; j < rows; j++)
                {
                    tmp.arr[i][j] = arr[i][j] + Matrix1.arr[i][j];
                }
            }

            return tmp;
        }
        else { throw exception(); }
    }
    KMatrix operator + (const KMatrix& Matrix1) const
    {
        KMatrix tmp;
        tmp = Matrix1;
        if (columns == tmp.columns && rows == tmp.rows)
        {
            for (int i = 0; i < columns; i++)
            {
                for (int j = 0; j < rows; j++)
                {
                    tmp.arr[i][j] = arr[i][j] + Matrix1.arr[i][j];
                }
            }

            return tmp;
        }
        else { throw exception(); }
    }
    KMatrix& operator += (const KMatrix& Matrix1)
    {
        if (columns == Matrix1.columns && rows == Matrix1.rows) {
            for (int i = 0; i < columns; i++)
            {
                for (int j = 0; j < rows; j++)
                {
                    arr[i][j] += Matrix1.arr[i][j];
                }
            }
            return *this;
        }
        else { throw exception(); }
    }

    KMatrix operator - (int number)
    {
        for (int i = 0; i < rows; i++) {
            arr[i][i] -= number;
        }
        return *this;
    }
    KMatrix& operator -= (const KMatrix& Matrix1)
    {
        if (columns == Matrix1.columns && rows == Matrix1.rows) {
            for (int i = 0; i < rows; i++)
            {
                for (int j = 0; j < columns; j++)
                {
                    arr[i][j] -= Matrix1.arr[i][j];
                }
            }
            return *this;
        }
        else { throw exception(); }
    }

    KMatrix operator * (int number)
    {
        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < columns; j++)
            {
                arr[i][j] *= number;
            }
        }
        return *this;
    }
    KMatrix operator * (const KMatrix& m1)
    {
        if (columns != m1.rows) { throw exception(); }
        else {
            KMatrix m2(rows, m1.columns);
            if (columns != m1.rows) {
                static_assert("Вы не можете умножать эти матрицы");
            }
            for (int i = 0; i < rows; i++)
                for (int j = 0; j < m1.columns; j++)
                    for (int k = 0; k < columns; k++)
                    {
                        m2.arr[i][j] += arr[i][k] * m1.arr[k][j];
                    }
            return m2;
        }
    }
    KMatrix& operator *= (const KMatrix& m1)
    {
        if (columns != m1.rows) { throw exception(); }
        else {
            KMatrix m2(rows, m1.columns);
            m2 = *this * m1;
            *this = m2;
            return *this;
        }
    }

    KMatrix& operator = (const KMatrix& Matr)
    {   
        if (*this == Matr) { return *this; } 
        else {
            if (columns > 0)
            {
                for (int i = 0; i < rows; i++)
                    delete[] arr[i];
            }
            if (rows > 0)
            {
                delete[] arr;
            }
            columns = Matr.columns;
            rows = Matr.rows;
            arr = (int**) new int* [rows];
            for (int i = 0; i < rows; i++)
                arr[i] = (int*) new int[columns];

            for (int i = 0; i < rows; i++)
                for (int j = 0; j < columns; j++)
                    arr[i][j] = Matr.arr[i][j];
            return *this;
        }
    }
    KMatrix& operator = (KMatrix&& Matr) noexcept {
        // TODO Лучше привлечь перемещающий конструктор копирования для реализации =
        cout << "Move assigment has been called\n";
        arr = Matr.arr;
        Matr.arr = nullptr;
        return (*this);

    }
    int* operator [] (int number) const
    {
        if (columns >= number && rows >= number) {
            return arr[number - 1];
        }
        else {
            throw exception();
        }
    }

    friend ostream& operator << (ostream& out, const KMatrix& x)
    {
        for (int i = 0; i < x.rows; i++)
        {
            for (int j = 0; j < x.columns; j++)
                cout << x.arr[i][j] << '\t';
            cout << endl;
        }
        return out;
    }
    friend istream& operator >> (istream& input, const KMatrix& x)
    {
        cout << "Please fill your matrix with " << x.columns * x.rows << " elements" << endl;
        for (int i = 0; i < x.rows; i++) {
            for (int j = 0; j < x.columns; j++)
            {
                cin >> x.arr[i][j];
            }
        }
        return input;
    }
    friend istream& operator >> (istream& input, KMatrix& x)
    {
        cout << "Please fill your matrix with " << x.columns * x.rows << " elements" << endl;
        for (int i = 0; i < x.rows; i++) {
            for (int j = 0; j < x.columns; j++)
            {
                cin >> x.arr[i][j];
            }
        }
        return input;
    }

    iterator       begin() { return iterator(this, 0, 0); }
    iterator       end() { return iterator(this, 0, this->rows); }

    const_iterator begin() const { return const_iterator((const KMatrix<const T>*)this, 0, 0); } // Возвращает итератор на начало списка
    const_iterator end() const { return const_iterator((const KMatrix<const T>*)this, 0, this->rows); } // Возвращает итератор на начало списка

    const_iterator cbegin() { return  const_iterator((const KMatrix<const T>*)this, 0, 0); } // Возвращает итератор на начало списка
    const_iterator cbegin() const { return  const_iterator((const KMatrix<const T>*)this, 0, 0); } // Возвращает итератор на начало списка

        //iterator begin() { return iterator(this, fpFirst, &fpFirst); } // Возвращает итератор на начало списка
        //const_iterator begin() const { return const_iterator((const KList<const T>*)this, (KNode<const T>*)fpFirst, (KNode<const T>**) & fpFirst); } // Возвращает итератор на начало списка

        //iterator   end() { return iterator(this, nullptr, fppLast); }
        //const_iterator   end() const { return const_iterator((const KList<const T>*)this, (KNode<const T>*)nullptr, (KNode<const T>**)fppLast); }

        //const_iterator cbegin() { return  const_iterator((const KList<const T>*)this, (KNode<const T>*)fpFirst, (KNode<const T>**) & fpFirst); } // Возвращает итератор на начало списка
        //const_iterator cbegin() const { return  const_iterator((const KList<const T>*)this, (KNode<const T>*)fpFirst, (KNode<const T>**) & fpFirst); } // Возвращает итератор на начало списка
        //const_iterator   cend() { return  const_iterator((const KList<const T>*)this, (KNode<const T>*)nullptr, (KNode<const T>**)fppLast); }
        //const_iterator   cend() const { return  const_iterator((const KList<const T>*)this, (KNode<const T>*)nullptr, (KNode<const T>**)fppLast); }

};

template <typename T>
//inline int& KIterMatr<T>::operator *() { // TODO
inline T& KIterMatr<T>::operator *() {
    if (*this == pArr->end()) { cout << "Hello\n"; throw exception(); }
    else
        return pArr->arr[Row][Col];
}

template <typename T>
//inline int const& KIterMatr<T>::operator *() const {
inline T const& KIterMatr<T>::operator *() const {
    if (*this == pArr->end()) { throw exception(); }
    else
        return (*pArr).arr[Row][Col];
}

template<typename T>
KIterMatr<T>::KIterMatr(KMatrix<T>* pArr_, int n, int m) : pArr(pArr_), Row(m), Col(n)
{
    //Col = n;
    //Row = m;
    //pArr = pArr_;
}
template<typename T>
KIterMatr<T>::KIterMatr(const KMatrix<T>* pArr_, int n, int m) : pArr((KMatrix<T>*)pArr_), Row(m), Col(n) {}

template<typename T>
KIterMatr<T>& KIterMatr<T>::operator ++()
{
    Col++;
    Col = Col % pArr->columns;
    if (Col == 0) { Row++; }
    return *this;
}
template<typename T>
KIterMatr<T> const  KIterMatr<T>::operator ++(int) { KIterMatr<T> t(*this); ++* this; return t; }

template<typename T>
bool KIterMatr<T>::operator == (KIterMatr<T> const& it2) const { return Col == it2.Col && Row == it2.Row; }
template<typename T>
bool KIterMatr<T>::operator != (KIterMatr<T> const& it2) const { return !(it2 == *this); }

int main()
{
    setlocale(0, "RUS");
    KMatrix<int> m(3,3);
    cout << m;
    KMatrix<int> m1 = { {1,2,3,4}, {5,6,7,8} };
    cout << "\nЦикл for:\n";
    for (auto c : m1) { cout << c << ' '; }
    cout << "\nПробегаем цикл через итератор:\n";
    try {
        for (auto it = m1.begin(); it != m1.end(); ++it)
        {
            cout << *it << ' ';
        }
        cout << "\nWe are here\n";
    }
    catch (exception())
    {
        cout << "Вы вышли за границы\n";
    }
    system("pause");

    std::cout << "\n";
    int min = *min_element(m1.begin(), m1.end());
    cout << "min: " << min << '\n';
    int max = *max_element(m1.begin(), m1.end());
    cout << "max: " << max << '\n';
    int f = *find(m1.begin(), m1.end(), 4);
    cout << "\nfind: " << f << '\n';




    //const KMatrix<int> m_21 = { {1,2,3},{4,5,6} };
    //for (auto const& x : m_21) { cout << x << " "; } 
    //cout << endl;                                     
    //cout << "------------------------------------------------" << endl;
    //KIterMatr it = m_21.cbegin(); 
    ////auto it = m_21.cbegin();
    //cout << *it << endl;


    // return 0;



     // проверка emplace, move и т.д.

     //KMatrix<int> m4{
     //    {1,2,7},
     //    {3,4,4},
     //    {5,5,5},
     //    {7,7,7}
     //};
     //KMatrix<int> m1(1, 1);
     //int n1, m111;
     //cout << "\n Enter n m\n ";
     //cin >> n1 >> m111;
     //KMatrix<int> m2(n1, m111);
     //cin >> m2;
     //chrono::time_point<chrono::system_clock> start, end;
     //start = chrono::system_clock::now();
     //KMatrix<int> a(m1);
     //end = chrono::system_clock::now();
     //auto elapsed_seconds1 = chrono::duration_cast<chrono::microseconds>(end - start).count();
     //
     //std::cout << '\n' << "KMatrix b=move(m2);\n";
     //
     //start = chrono::system_clock::now();
     //KMatrix<int> b( move(m2));
     //end = chrono::system_clock::now();
     //cout << b;
     //auto elapsed_seconds2 = chrono::duration_cast<chrono::microseconds>(end - start).count();
     //
     //
     //cout << "Copy constructorr time: " << elapsed_seconds1 << '\n';
     //cout << "Move constructorr time: " << elapsed_seconds2 << '\n';
     //try {
     //    b.change_element_emplace(1, 1, 12);
     //}
     //catch (int) { std::cout << "You can't change this element\n"; }
     //cout << b << '\n';
     ////cout <<m2 <<'\n';
     //cout << '\n';
     //cout << '\n';
     //// проверка emplace метода
     //{
     //    KMatrix<MyClass> M1(2, 2);
     //    MyClass m11(3);
     //    MyClass m01;
     //    M1.change_element_emplace(0, 1, 3,1);
     //    cout << M1 << '\n';
     //    M1.change_element_emplace(1, 1, m11);
     //    cout << M1;
     //}

};