#include <iostream>
#include <algorithm>
#include <ctime>
#include <tuple>
#include <functional>

class Treap{
private:

    static int myRand_(){
        static std::mt19937 generator;
        static std::uniform_int_distribution <int> distributor;
        return distributor(generator);
    }

    struct Vertex_ {
        int priority_;
        long long key_, LeftKey_, RightKey_, PushSubtree_, SummSubtree_;
        bool isOrdered_[2];
        bool isReverse_;
        bool isSetZero_;
        long long Size_;
        Vertex_ *left_;
        Vertex_ *right_;

        Vertex_(long long Key) : priority_(myRand_()), key_(Key), LeftKey_(Key), RightKey_(Key),
                                 isReverse_(false), isSetZero_(false),
                                 SummSubtree_(key_), PushSubtree_(0), Size_(1), left_(nullptr), right_(nullptr) {
            isOrdered_[0] = true;
            isOrdered_[1] = true;
        }

        ~Vertex_() {
            delete left_;
            delete right_;
        }
    };

    static long long Size_(Vertex_ *theFirst){
        return (!theFirst ? 0 : theFirst->Size_);
    }

    static void updateAdd_(Vertex_ *theFirst, const long long &x) {
        if (!theFirst)
            return;
        theFirst->key_ += x;
        theFirst->LeftKey_ += x;
        theFirst->RightKey_ += x;
        theFirst->SummSubtree_ += x * Size_(theFirst);

    }

    static void updateSetZero_(Vertex_ *theFirst) {
        if (theFirst == nullptr)
            return;
        theFirst->key_ = theFirst->LeftKey_ = theFirst->RightKey_ = theFirst->SummSubtree_ = 0;
        theFirst->PushSubtree_ = 0;
        theFirst->isOrdered_[0] = theFirst->isOrdered_[1] = true;
    }

    static void pushReverse_(Vertex_ *theFirst){
        std::swap(theFirst->isOrdered_[0], theFirst->isOrdered_[1]);
        std::swap(theFirst->LeftKey_, theFirst->RightKey_);
        std::swap(theFirst->left_, theFirst->right_);
        theFirst->isReverse_ = false;
        for (auto &i:{theFirst->left_, theFirst->right_})
            if (i)
                i->isReverse_ ^= true;
    }

    static void pushSetZero_(Vertex_ *theFirst) {
        for (auto &i:{theFirst->left_, theFirst->right_}){
            updateSetZero_(i);
            if(i)
                i -> isSetZero_= true;
        }
        theFirst->isSetZero_ = false;
    }

    static void pushSubtree_(Vertex_ *theFirst) {
        for (auto &i:{theFirst->left_, theFirst->right_}){
            updateAdd_(i, theFirst->PushSubtree_);
            if (i)
                i->PushSubtree_ += theFirst->PushSubtree_;
        }
        theFirst->PushSubtree_ = 0;
    }

    static void push_(Vertex_ *theFirst) {
        if (theFirst == nullptr) return;
        if (theFirst->isReverse_) {
            pushReverse_(theFirst);
        }
        if (theFirst->isSetZero_) {
            pushSetZero_(theFirst);
        }
        if (theFirst->PushSubtree_ != 0) {
            pushSubtree_(theFirst);
        }
    }

    static void update_(Vertex_ *theFirst) {
        if (theFirst == nullptr)
            return;
        push_(theFirst->left_);
        push_(theFirst->right_);
        theFirst->Size_ = 1 + Size_(theFirst->left_) + Size_(theFirst->right_);
        for (int i = 0; i < 2; ++i)
            theFirst->isOrdered_[i] = (theFirst->left_ ? theFirst->left_->isOrdered_[i] : true) &
                                  (theFirst->right_ ? theFirst->right_->isOrdered_[i] : true) &
                                  (theFirst->left_ ? (i ^ theFirst->left_->RightKey_ < theFirst->key_) ||
                                          theFirst->left_->RightKey_ == theFirst->key_ : true) &
                                  (theFirst->right_ ? (i ^ theFirst->key_ < theFirst->right_->LeftKey_) ||
                                          theFirst->key_ == theFirst->right_->LeftKey_ : true);

        theFirst->LeftKey_ = (theFirst->left_ ? theFirst->left_->LeftKey_ : theFirst->key_);
        theFirst->RightKey_ = (theFirst->right_ ? theFirst->right_->RightKey_ : theFirst->key_);
        theFirst->SummSubtree_ = theFirst->key_ + (theFirst->left_ ? theFirst->left_->SummSubtree_ : 0LL) +
                                 (theFirst->right_ ? theFirst->right_->SummSubtree_ : 0LL);
    }

    static std :: pair <Vertex_ *, Vertex_ *> split_(Vertex_ *theFirst, long long key, long long add = 0) {
        Vertex_ *r, *l;
        if (theFirst == nullptr) {
            r = nullptr;
            l = nullptr;
            return std :: make_pair(l, r);
        }
        push_(theFirst);
        long long curKey = add + Size_(theFirst->left_);
        if (key <= curKey) {
            std::tie(l, theFirst->left_) = split_(theFirst->left_, key, add);
            r = theFirst;
        } else {
            std :: tie(theFirst->right_, r) = split_(theFirst->right_, key, add + 1 + Size_(theFirst->left_));
            l = theFirst;
        }
        update_(theFirst);
        return std :: make_pair(l,r);
    }
    template <typename Comp>
    static std :: pair <Vertex_ *, Vertex_ *> split_key_(Vertex_ *theFirst, long long key, Comp comp) {
        Vertex_ *r, *l;
        if (theFirst == nullptr) {
            r = nullptr;
            l = nullptr;
            return std :: make_pair(l, r);
        }
        push_(theFirst);
        //if (key < theFirst->key_) {
        if(comp(key, theFirst->key_)){
            std :: tie(l, theFirst->left_) = split_key_(theFirst->left_, key, comp);
            r = theFirst;
        } else {
            std :: tie(theFirst->right_, r) = split_key_(theFirst->right_, key, comp);
            l = theFirst;
        }
        update_(theFirst);
        return std :: make_pair(l, r);
    }

    static Vertex_ *merge_(Vertex_ *l, Vertex_ *r) {
        push_(l);
        push_(r);
        if (l == nullptr)
            return r;
        else if (r == nullptr)
            return l;
        else if (l->priority_ > r->priority_) {
            l->right_ = merge_(l->right_, r);
            update_(l);
            return l;
        } else {
            r->left_ = merge_(l, r->left_);
            update_(r);
            return r;
        }
    }

    static Vertex_ *merge_(std::vector<Vertex_ *> Vertexes){
        for (size_t i = 1; i < Vertexes.size(); ++i) {
            Vertexes[0] = merge_(Vertexes[0], Vertexes[i]);
        }
        return Vertexes[0];
    }

    static long long findLenOrderedSuffix_(Vertex_ *theFirst, bool notReversed) {
        if (theFirst == nullptr)
            return 0;
        push_(theFirst);
        for(auto &i:{theFirst->left_, theFirst->right_}){
            push_(i);
            if(i){
                for(auto &j:{i->left_, i->right_}){
                     push_(j);
                }
            }
        }
        if (theFirst->right_ && !theFirst->right_->isOrdered_[!notReversed]) {
            return 1 + Size_(theFirst->left_) + findLenOrderedSuffix_(theFirst->right_, notReversed);
        }
        if ( (notReversed)? (theFirst->right_ && theFirst->key_ > theFirst->right_->LeftKey_) :
             (theFirst->right_ && theFirst->key_ < theFirst->right_->LeftKey_)) {
            return 1 + Size_(theFirst->left_);
        }
        if ((notReversed)? (theFirst->left_ && theFirst->key_ < theFirst->left_->RightKey_) :
            (theFirst->left_ && theFirst->key_ > theFirst->left_->RightKey_) ) {
            return Size_(theFirst->left_);
        }
        return findLenOrderedSuffix_(theFirst->left_, notReversed);
    }

    static void setReversing_(Vertex_ *theFirst){
        if (theFirst == nullptr)
            return;
        theFirst -> isReverse_ = true;
        push_(theFirst);
    }

    template<bool isOrdered>
    static Vertex_ *permutation_(Vertex_ *theFirst){
        if (theFirst->isOrdered_[isOrdered]) {
            setReversing_(theFirst);
            return theFirst;
        }
        Vertex_ *treeLeft, *NowNode, *treeRight;
        long long len = findLenOrderedSuffix_(theFirst, !isOrdered);
        std :: tie(treeLeft, treeRight) = split_(theFirst, len - 1);
        std :: tie(NowNode, treeRight) = split_(treeRight, 1);
        Vertex_ *treeRight_Left, *treeRight_Right, *SwapElement;
        setReversing_(treeRight);
        std::tie(treeRight_Left, treeRight_Right) = isOrdered ?
                                                    split_key_(treeRight, NowNode->key_, std::less<long long>()):
                                                    split_key_(treeRight, NowNode->key_, std::greater<long long>());
        std::tie(SwapElement, treeRight_Right) = split_(treeRight_Right, 1);
        theFirst = merge_({treeLeft, SwapElement, treeRight_Left, NowNode, treeRight_Right});
        return theFirst;
    }

    static void out_(Vertex_ *theFirst, std::vector<long long> &Result) {
        if (theFirst == nullptr)
            return;
        push_(theFirst);
        out_(theFirst->left_, Result);
        Result.push_back(theFirst->key_);
        out_(theFirst->right_, Result);
    }

    Vertex_ *Root_;

    template<typename Function>
    Vertex_ * makeOperation_(long long l, long long r, Function f){
        Vertex_ *first, *second, *third;
        std :: tie(second, third) = split_(Root_, r + 1);
        std :: tie(first, second) = split_(second, l);

        second = f(second);

        Root_ = merge_({first, second, third});
    }

public:

    Treap() : Root_(nullptr){}

    ~Treap(){
        delete Root_;
    }

    long long SummInSegment( long long l, long long r) {
        long long Summ = 0;
        makeOperation_(l, r, [&Summ](Vertex_ *subTree) -> Vertex_ * {
            Summ = subTree->SummSubtree_;
            return subTree;
        });

        return Summ;
    }

    void Insert( int pos, long long x) {
        makeOperation_(pos, pos - 1, [this, &x](Vertex_ *subTree) -> Vertex_ * {
            subTree = new Vertex_(x);
            return subTree;
        });
    }

    void Remove( long long pos) {
        makeOperation_(pos, pos, [](Vertex_ *subTree) -> Vertex_ * {
            delete subTree;
            return nullptr;
        });
    }

    void Add( long long x, long long l, long long r) {
        makeOperation_(l, r, [this, &x](Vertex_ *subTree) -> Vertex_ * {
            subTree->PushSubtree_ += x;
            updateAdd_(subTree, x);
            return subTree;
        });
    }

    void Set( long long x, long long l, long long r) {
        makeOperation_(l, r, [this, &x](Vertex_ *subTree) -> Vertex_ * {

            subTree->isSetZero_ = true;
            updateSetZero_(subTree);
            subTree->PushSubtree_ = x;
            updateAdd_(subTree, x);
            return subTree;
        });
    }

    void NextPermutation(long long l, long long r){
        makeOperation_(l, r, [this](Vertex_ *subTree) -> Vertex_ * {
            subTree = permutation_<true>(subTree);
            return subTree;
        });
    }

    void PrevPermutation(long long l, long long r){
        makeOperation_(l, r, [this](Vertex_ *subTree) -> Vertex_ * {
            subTree = permutation_<false>(subTree);
            return subTree;
        });
    }

    std :: vector<long long> GetArray() {
        std::vector<long long> all;
        out_(Root_, all);
        return all;
    }
};

enum OperationName{
    SummInSegment = 1,
    Insert,
    Remove,
    Set,
    Add,
    NextPermutation,
    PrevPermutation
};

class Operation{
protected:
    std::vector<long long> data_;
public:
    Operation(std::vector<long long> &input): data_(input){};
    Operation() = default;
    ~Operation() = default;

    virtual int GetPos()const = 0;
    virtual long long GetLeft()const = 0;
    virtual long long GetRight()const = 0;
    virtual long long GetKey()const = 0;
    virtual OperationName GetName()const = 0;
};

class OperationRemove : virtual public Operation{
public:
    OperationRemove(std::vector<long long> &input) : Operation(input){}
    int GetPos() const{
        return int(data_[0]);
    }

    long long GetLeft()const{}
    long long GetRight()const {}
    long long GetKey()const {}

    OperationName GetName()const{
        return Remove;
    };
};


class OperationInsert : virtual public Operation{
public:
    OperationInsert(std::vector<long long> &input) : Operation(input){}
    int GetPos() const{
        return int(data_[0]);
    }

    long long GetKey() const{
        return data_[1];
    }

    long long GetLeft()const {}
    long long GetRight()const {}

    OperationName GetName()const{
        return Insert;
    };
};

class OperationLeftAndRight : virtual public Operation{
public:
    OperationLeftAndRight() = default;
    OperationLeftAndRight(std::vector<long long> &input) : Operation(input){}
    int GetPos()const{}

    long long GetKey()const {}

    long long GetLeft() const{
        return data_[0];
    }

    long long GetRight() const {
        return data_[1];
    }

    virtual OperationName GetName()const = 0;
};

class OperationKeyAndLeftAndRight : virtual public Operation{
public:
    OperationKeyAndLeftAndRight() = default;
    OperationKeyAndLeftAndRight(std::vector<long long> &input) : Operation(input){}
    int GetPos() const {}
    long long GetKey() const{
        return data_[0];
    }

    long long GetLeft() const{
        return data_[1];
    }

    long long GetRight() const{
        return data_[2];
    }

    virtual OperationName GetName()const = 0;
};

class OperationSumm : public OperationLeftAndRight{
public:
    OperationSumm(std::vector<long long> &input) : Operation(input){}
    OperationName GetName()const{
        return SummInSegment;
    }
};

class OperationPrevPerm : public OperationLeftAndRight{
public:
    OperationPrevPerm(std::vector<long long> &input) : Operation(input){}
    OperationName GetName()const{
        return PrevPermutation;
    }
};


class OperationNextPerm : public OperationLeftAndRight{
public:
    OperationNextPerm(std::vector<long long> &input) : Operation(input){}
    OperationName GetName()const{
        return NextPermutation;
    }
};

class OperationSet : public OperationKeyAndLeftAndRight{
public:
    OperationSet(std::vector<long long> &input) : Operation(input){}
    OperationName GetName()const{
        return Set;
    }
};

class OperationAdd : public OperationKeyAndLeftAndRight{
public:
    OperationAdd(std::vector<long long> &input) : Operation(input){}
    OperationName GetName()const{
        return Add;
    }
};

struct Input{
    std :: vector <long long> InputTreap;
    std :: vector<Operation*> array;
};

struct Output{
    std :: vector <long long> Results;
    std :: vector <long long> OutputTreap;
};

Input getInput(std :: istream& in) {
    Input input;
    size_t countOfVertex, countOfQuestions;
    int i;
    long long type, pos, left, right, Key;
    in >> countOfVertex;
    for (i = 0; i < countOfVertex; ++i) {
        in >> Key;
        input.InputTreap.push_back(Key);
    }
    in >> countOfQuestions;
    std :: vector<long long> q;
    OperationSumm* tSum;
    OperationSet* tSet;
    OperationPrevPerm* tPre;
    OperationNextPerm* tNex;
    OperationInsert* tIns;
    OperationRemove* tRem;
    OperationAdd* tAdd;
    for (i = 0; i < countOfQuestions; ++i) {
        in >> type;
        switch (type) {
            case SummInSegment:
                in >> left >> right;
                q = {left, right};
                tSum = new OperationSumm(q);
                input.array.push_back(tSum);
                break;
            case Insert:
                in >> Key >> pos;
                q = {pos, Key};
                tIns = new OperationInsert(q);
                input.array.push_back(tIns);
                break;
            case Remove:
                in >> pos;
                q = {pos};
                tRem = new OperationRemove(q);
                input.array.push_back(tRem);
                break;
            case Set:
                in >> Key >> left >> right;
                q = {Key, left, right};
                tSet = new OperationSet(q);
                input.array.push_back(tSet);
                break;
            case Add:
                in >> Key >> left >> right;
                q = {Key, left, right};
                tAdd = new OperationAdd(q);
                input.array.push_back(tAdd);
                break;
            case NextPermutation:
                in >> left >> right;
                q = {left, right};
                tNex = new OperationNextPerm(q);
                input.array.push_back(tNex);
                break;
            case PrevPermutation:
                in >> left >> right;
                q = {left, right};
                tPre = new OperationPrevPerm(q);
                input.array.push_back(tPre);
                break;
            default:
                break;
        }
    }
    return input;
}

Output doOperations(const Input& input){
    Treap treap;
    int j = 1;
    for(auto i:input.InputTreap){
        treap.Insert(j, i);
        ++j;
    }
    Output output;
    for(auto i:input.array){
        switch (i->GetName()) {
            case SummInSegment:
                output.Results.push_back(treap.SummInSegment(i->GetLeft(), i->GetRight()));
                break;
            case Insert:
                treap.Insert(i->GetPos(), i->GetKey());
                break;
            case Remove:
                treap.Remove(i->GetPos());
                break;
            case Set:
                treap.Set(i->GetKey(), i->GetLeft(), i->GetRight());
                break;
            case Add:
                treap.Add(i->GetKey(), i->GetLeft(), i->GetRight());
                break;
            case NextPermutation:
                treap.NextPermutation(i->GetLeft(), i->GetRight());
                break;
            case PrevPermutation:
                treap.PrevPermutation(i->GetLeft(), i->GetRight());
                break;
            default:
                break;
        }
    }
    for ( auto &i:input.array){
        i -> ~Operation();
    }
    output.OutputTreap = treap.GetArray();
    return output;
}

void PrintArr(const std :: vector<long long>& arr, char sep, std :: ostream& out){
    bool flag = false;
    for(auto i:arr){
        flag? out << sep: flag = true;
        out << i;
    }
    out << std :: endl;
}

void doOutput(const Output& output, std :: ostream& out){
    PrintArr(output.Results, '\n', out);
    PrintArr(output.OutputTreap, ' ', out);
}

void run(std :: istream& in, std :: ostream& out) {
    Input get = getInput(in);
    Output make = doOperations(get);
    doOutput(make, out);
}

int main(){
    run(std::cin, std::cout);
}