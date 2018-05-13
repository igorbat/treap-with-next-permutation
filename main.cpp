#include <iostream>
#include <algorithm>
#include <ctime>

class Treap{
private:
    struct Vertex_ {
        long long priority_;
        long long key_, LeftKey_, RightKey_, PushSubtree_, SummSubtree_;
        bool isOrdered_, isReverseOrdered_;
        bool isReverse;
        bool isSetZero;
        long long Size_;
        Vertex_ *left_;
        Vertex_ *right_;

        Vertex_(long long Key) : priority_(rand()), key_(Key), LeftKey_(Key), RightKey_(Key),
                                 isOrdered_(true), isReverseOrdered_(true), isReverse(false), isSetZero(false),
                                 SummSubtree_(key_), PushSubtree_(0), Size_(1), left_(nullptr), right_(nullptr) {}

        ~Vertex_() {
            delete left_;
            delete right_;
        }
    };

    long long Size(Vertex_ *t) {
        return (!t ? 0 : t->Size_);
    }

    void update_Add(Vertex_ *theFirst, long long x) {
        if (!theFirst)return;
        theFirst->key_ += x;
        theFirst->LeftKey_ += x;
        theFirst->RightKey_ += x;
        theFirst->SummSubtree_ += x * Size(theFirst);

    }

    void update_SetZero(Vertex_ *theFirst) {
        if (theFirst == nullptr) return;
        theFirst->key_ = theFirst->LeftKey_ = theFirst->RightKey_ = theFirst->SummSubtree_ = 0;
        theFirst->PushSubtree_ = 0;
        theFirst->isOrdered_ = theFirst->isReverseOrdered_ = true;
    }

    void push(Vertex_ *theFirst) {
        if (theFirst == nullptr) return;
        if (theFirst->isReverse) {
            std::swap(theFirst->isOrdered_, theFirst->isReverseOrdered_);
            std::swap(theFirst->LeftKey_, theFirst->RightKey_);
            std::swap(theFirst->left_, theFirst->right_);
            theFirst->isReverse = false;
            if (theFirst->left_)theFirst->left_->isReverse ^= true;
            if (theFirst->right_)theFirst->right_->isReverse ^= true;
        }
        if (theFirst->isSetZero) {
            update_SetZero(theFirst->left_);
            update_SetZero(theFirst->right_);
            if (theFirst->left_)theFirst->left_->isSetZero = true;
            if (theFirst->right_)theFirst->right_->isSetZero = true;
            theFirst->isSetZero = false;
        }
        if (theFirst->PushSubtree_ != 0) {
            update_Add(theFirst->left_, theFirst->PushSubtree_);
            update_Add(theFirst->right_, theFirst->PushSubtree_);
            if (theFirst->left_)theFirst->left_->PushSubtree_ += theFirst->PushSubtree_;
            if (theFirst->right_)theFirst->right_->PushSubtree_ += theFirst->PushSubtree_;
            theFirst->PushSubtree_ = 0;
        }
    }

    void update(Vertex_ *theFirst) {
        if (theFirst == nullptr) return;
        push(theFirst->left_);
        push(theFirst->right_);
        theFirst->Size_ = 1 + Size(theFirst->left_) + Size(theFirst->right_);
        theFirst->isOrdered_ = (theFirst->left_ ? theFirst->left_->isOrdered_ : true) &
                               (theFirst->right_ ? theFirst->right_->isOrdered_ : true) &
                               (theFirst->left_ ? theFirst->left_->RightKey_ <= theFirst->key_ : true) &
                               (theFirst->right_ ? theFirst->key_ <= theFirst->right_->LeftKey_ : true);
        theFirst->isReverseOrdered_ = (theFirst->left_ ? theFirst->left_->isReverseOrdered_ : true) &
                                      (theFirst->right_ ? theFirst->right_->isReverseOrdered_ : true) &
                                      (theFirst->left_ ? theFirst->left_->RightKey_ >= theFirst->key_ : true) &
                                      (theFirst->right_ ? theFirst->key_ >= theFirst->right_->LeftKey_ : true);

        theFirst->LeftKey_ = (theFirst->left_ ? theFirst->left_->LeftKey_ : theFirst->key_);
        theFirst->RightKey_ = (theFirst->right_ ? theFirst->right_->RightKey_ : theFirst->key_);
        theFirst->SummSubtree_ = theFirst->key_ + (theFirst->left_ ? theFirst->left_->SummSubtree_ : 0LL) +
                                 (theFirst->right_ ? theFirst->right_->SummSubtree_ : 0LL);
    }

    std :: pair <Vertex_ *, Vertex_ *> split(Vertex_ *theFirst, long long key, long long add = 0) {
        Vertex_ *r, *l;
        std :: pair <Vertex_ *, Vertex_ *> temp;
        if (theFirst == nullptr) {
            r = nullptr;
            l = nullptr;
            return std :: make_pair(l, r);
        }
        push(theFirst);
        long long curKey = add + Size(theFirst->left_);
        if (key <= curKey) {
            temp = split(theFirst->left_, key, add);
            l = temp.first;
            theFirst->left_ = temp.second;
            r = theFirst;
        } else {
            temp = split(theFirst->right_, key, add + 1 + Size(theFirst->left_));
            theFirst->right_ = temp.first;
            r = temp.second;
            l = theFirst;
        }
        update(theFirst);
        return std :: make_pair(l,r);
    }

    void split(Vertex_ *theFirst, Vertex_ *&l, Vertex_ *&r, long long key, long long add = 0) {
        if (theFirst == nullptr) {
            r = nullptr;
            l = nullptr;
            return;
        }
        push(theFirst);
        long long curKey = add + Size(theFirst->left_);
        if (key <= curKey) {
            split(theFirst->left_, l, theFirst->left_, key, add);
            r = theFirst;
        } else {
            split(theFirst->right_, theFirst->right_, r, key, add + 1 + Size(theFirst->left_));
            l = theFirst;
        }
        update(theFirst);
    }

    void split_key(Vertex_ *theFirst, Vertex_ *&l, Vertex_ *&r, long long key) {
        if (theFirst == nullptr) {
            r = nullptr;
            l = nullptr;
            return;
        }
        push(theFirst);
        if (key < theFirst->key_) {
            split_key(theFirst->left_, l, theFirst->left_, key);
            r = theFirst;
        } else {
            split_key(theFirst->right_, theFirst->right_, r, key);
            l = theFirst;
        }
        update(theFirst);
    }
    Vertex_ *merge(Vertex_ *l, Vertex_ *r) {
        push(l);
        push(r);
        if (l == nullptr)return r;
        else if (r == nullptr)return l;
        else if (l->priority_ > r->priority_) {
            l->right_ = merge(l->right_, r);
            update(l);
            return l;
        } else {
            r->left_ = merge(l, r->left_);
            update(r);
            return r;
        }
    }

    long long FindLenOrderedSuffix(Vertex_ *t) {
        if (t == nullptr)return 0;
        push(t);
        push(t->left_);
        push(t->right_);
        if (t->left_) {
            push(t->left_->left_);
            push(t->left_->right_);
        }
        if (t->right_) {
            push(t->right_->left_);
            push(t->right_->right_);
        }
        if (t->right_ && !t->right_->isOrdered_) {
            return 1 + Size(t->left_) + FindLenOrderedSuffix(t->right_);
        }
        if (t->right_ && t->key_ > t->right_->LeftKey_) {
            return 1 + Size(t->left_);
        }
        if (t->left_ && t->key_ < t->left_->RightKey_) {
            return Size(t->left_);
        }
        return FindLenOrderedSuffix(t->left_);
    }

    long long FindLenReverseOrderedSuffix(Vertex_ *theFirst) {
        if (theFirst == nullptr)return 0;
        push(theFirst);
        push(theFirst->left_);
        push(theFirst->right_);
        if (theFirst->left_) {
            push(theFirst->left_->left_);
            push(theFirst->left_->right_);
        }
        if (theFirst->right_) {
            push(theFirst->right_->left_);
            push(theFirst->right_->right_);
        }
        if (theFirst->right_ && !theFirst->right_->isReverseOrdered_) {
            return 1 + Size(theFirst->left_) + FindLenReverseOrderedSuffix(theFirst->right_);
        }
        if (theFirst->right_ && theFirst->key_ < theFirst->right_->LeftKey_) {
            return 1 + Size(theFirst->left_);
        }
        if (theFirst->left_ && theFirst->key_ > theFirst->left_->RightKey_) {
            return Size(theFirst->left_);
        }
        return FindLenReverseOrderedSuffix(theFirst->left_);
    }
    void out(Vertex_ *theFirst, std::vector<long long> &Result) {
        if (theFirst == nullptr) return;
        push(theFirst);
        out(theFirst->left_, Result);
        Result.push_back(theFirst->key_);
        out(theFirst->right_, Result);
    }

    Vertex_ * Root;
public:
    Treap() : Root(nullptr){}
    ~Treap(){
        delete Root;
    }

    long long SummInSegment( long long l, long long r) {
        Vertex_ *first, *second, *third;
        split(Root, second, third, r + 1);
        split(second, first, second, l);
        long long Summ = second->SummSubtree_;
        Root = merge(merge(first, second), third);
        return Summ;
    }

    void Insert( int pos, long long x) {
        Vertex_ *first, *second;
        split(Root, first, second, pos);
        Root = merge(merge(first, new Vertex_(x)), second);
    }

    void Remove( long long pos) {
        Vertex_ *first, *second, *third;
        split(Root, first, second, pos);
        split(second, second, third, 1);
        delete second;
        Root = merge(first, third);
    }

    void Add( long long x, long long l, long long r) {
        Vertex_ *first, *second, *third;
        split(Root, second, third, r + 1);
        split(second, first, second, l);
        second->PushSubtree_ += x;
        update_Add(second, x);
        Root = merge(merge(first, second), third);
    }

    void Set( long long x, long long l, long long r) {
        Vertex_ *first, *second, *third;
        split(Root, second, third, r + 1);
        split(second, first, second, l);
        second->isSetZero = true;
        update_SetZero(second);
        second->PushSubtree_ = x;
        update_Add(second, x);
        Root = merge(merge(first, second), third);
    }

    void NextPermutation( long long l, long long r) {
        Vertex_ *RootLeft, *RootRight;
        split(Root, Root, RootRight, r + 1);
        split(Root, RootLeft, Root, l);
        if (Root->isReverseOrdered_) {
            Root->isReverse = true;
            Root = merge(merge(RootLeft, Root), RootRight);
            return;
        }

        Vertex_ *tLeft, *NowNode, *tRight;
        long long len = FindLenReverseOrderedSuffix(Root);
        split(Root, tLeft, tRight, len - 1);
        split(tRight, NowNode, tRight, 1);
        tRight->isReverse = true;
        Vertex_ *tRight_Left, *tRight_Right, *SwapElement;
        split_key(tRight, tRight_Left, tRight_Right, NowNode->key_);
        split(tRight_Right, SwapElement, tRight_Right, 1);
        Root = merge(merge(merge(merge(tLeft, SwapElement), tRight_Left), NowNode), tRight_Right);

        Root = merge(merge(RootLeft, Root), RootRight);
    }

    void PrevPermutation( long long l, long long r) {
        Vertex_ *RootLeft, *RootRight;
        split(Root, Root, RootRight, r + 1);
        split(Root, RootLeft, Root, l);
        if (Root->isOrdered_) {
            Root->isReverse = true;
            Root = merge(merge(RootLeft, Root), RootRight);
            return;
        }

        long long len = FindLenOrderedSuffix(Root);
        Vertex_ *tLeft, *NowNode, *tRight;
        split(Root, tLeft, tRight, len - 1);
        split(tRight, NowNode, tRight, 1);
        Vertex_ *tRight_Left, *tRight_Right, *SwapElement;
        split_key(tRight, tRight_Left, tRight_Right, NowNode->key_ - 1);
        split(tRight_Left, tRight_Left, SwapElement, Size(tRight_Left) - 1);

        if (tRight_Right)tRight_Right->isReverse = true;
        if (tRight_Left)tRight_Left->isReverse = true;
        Root = merge(merge(merge(merge(tLeft, SwapElement), tRight_Right), NowNode), tRight_Left);

        Root = merge(merge(RootLeft, Root), RootRight);
    }

    void print () {
        std::vector<long long> all;
        out(Root, all);
        for (long long x : all) {
            std::cout << x << " ";
        }
        std::cout << std::endl;
    }
};

int main() {
    srand((unsigned) time(NULL));
    Treap treap = Treap();
    int n,i,m;
    long long type, x, pos, l, r;
    std::cin >> n;
    for ( i = 0; i < n; ++i) {
        std::cin >> x;
        treap.Insert(i, x);
    }
    std::cin >> m;
    for (i = 1; i <= m; ++i) {

        std::cin >> type;
        switch (type) {
            case 1:
                std::cin >> l >> r;
                std::cout << treap.SummInSegment(l, r) << std::endl;
                break;
            case 2:
                std::cin >> x >> pos;
                treap.Insert(pos, x);
                break;
            case 3:
                std::cin >> pos;
                treap.Remove(pos);
                break;
            case 4:
                std::cin >> x >> l >> r;
                treap.Set(x, l, r);
                break;
            case 5:
                std::cin >> x >> l >> r;
                treap.Add(x, l, r);
                break;
            case 6:
                std::cin >> l >> r;
                treap.NextPermutation(l, r);
                break;
            case 7:
                std::cin >> l >> r;
                treap.PrevPermutation(l, r);
                break;
            default:
                break;
        }
    }
    treap.print();
}