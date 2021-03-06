#ifndef BUW_LIST_HPP
#define BUW_LIST_HPP
#include <cassert>
#include <cstddef>  //ptrdiff_t
#include <iterator> //std::bidirectional_iterator_tag
#include <iostream>

#include <initializer_list>

template <typename T>
class List;

template <typename T>
struct ListNode {
  T         value = T{};
  ListNode* prev = nullptr;
  ListNode* next = nullptr;
};


//TODO: Implementierung der Methoden des Iterators 
//      (nach Vorlesung STL-1 am 09. Juni) (Aufgabe 3.11)
template <typename T>
struct ListIterator {
  using Self              = ListIterator<T>;
  using value_type        = T;
  using pointer           = T*;
  using reference         = T&;
  using difference_type   = ptrdiff_t;
  using iterator_category = std::bidirectional_iterator_tag;


  /* Dereferenzieren, um auf den Knoten zugreifen zu können, auf den der Iterator zeigt*/
  //Dereferenzierung
  T&  operator*()  const {
    if(nullptr == node) {
      throw "Iterator does not point to valid node";
    }
    else{
      return node->value;
    }
  } 

  /* Dereferenzieren, um auf den Knoten zugreifen zu können, auf den der Iterator zeigt (z.B. in einer Methode) */
  T* operator->() const {
    if(nullptr == node) {
      throw "Iterator does not point to valid node";
    }
    else{
      return &(node->value);
      //should return the pointer of the value type
    }
  }


  /* PREINCREMENT, call: ++it, advances one element forward */
  ListIterator<T>& operator++() {
    if(nullptr == node) {
      throw "Iterator does not point to valid node";
    }
    else{
      node = node->next;
      return *this;
    }
  }


  //decrement (not needed), decrements the iterator +/
  ListIterator<T>& operator--() {
    if(nullptr == node) {
      throw "Iterator does not point to valid node";
    }
    else{
      node = node->prev;
      return *this;
    }
  }


  /* POSTINCREMENT (signature distinguishes the iterators), 
                    call:  it++, advances one element forward*/
  //returns original iterator!
  ListIterator<T> operator++(int) {
    if(nullptr == node) {
      throw "Iterator does not point to valid node";
    }
    else if(node->next == nullptr){ //reach the end iterator, i.e. the element past the end of the list
      ListNode <T>* original = node;
      node = nullptr;
      return {original};
    }
    else{
      node = node->next;
      return {node->prev};
    }
  }

  /*Equality-Operation for Iterator (Aufgabe 3.11 - Teil 5)
  Iterators should be the same if they refer to the same node*/
  bool operator==(ListIterator<T> const& x) const {
    if(node == x.node){
      return true;
    }
    return false;
  }

  //Inequality-Operation for Iterator (Aufgabe 3.11 - Teil 6)
  /* returns false if iterators refer to the same node */
  bool operator!=(ListIterator<T> const& x) const {
    if(operator==(x)){
      return false;
    }
    return true;
  } 



  /* Advances Iterator */
  ListIterator<T> next() const {
    if (nullptr != node) {
      return ListIterator{node->next};
    } else {
      return ListIterator{nullptr};
    }
  }


  ListNode <T>* node = nullptr;
};



template <typename T>
class List {
  public:

    //friend declarations for testing the members   
    template <typename TEST_TYPE>
    friend size_t get_size(List<TEST_TYPE> const& list_to_test);
    template <typename TEST_TYPE>
    friend ListNode<TEST_TYPE>* get_first_pointer(List<TEST_TYPE> const& list_to_test);
    template <typename TEST_TYPE>
    friend ListNode<TEST_TYPE>* get_last_pointer(List<TEST_TYPE> const& list_to_test);

    using value_type      = T;
    using pointer         = T*;
    using const_pointer   = T const*;
    using reference       = T&;
    using const_reference = T const&;
    using iterator        = ListIterator<T>;


    /* Default-Konstuktor */
    List():
      size_ {0},
      first_ {nullptr},
      last_ {nullptr}
     {}

    // Move-Konstruktor (Aufgabe 3.9)
    List(List<T>&& rhs):
      size_(rhs.size_),
      first_(rhs.first_), // Listenelemente von rhs uebernehmen
      last_(rhs.last_) { 
        rhs.size_ = 0;
        rhs.first_ = nullptr; // rhs leeren
        rhs.last_ = nullptr;
        move_aufrufe++;
      }
    
    int show_move_aufrufe() const{
      return move_aufrufe;
    }

    //TODO: Initializer-List Konstruktor (3.14 - Teil 1)
    /* @param Objekt vom Typ std::initializer_list<T> 
    Inhalte der initializer List werden in die neu konstruierte Liste einfügt*/
    List(std::initializer_list<T> ini_list):
      size_{0},
      first_{nullptr},
      last_{nullptr} {
      for (T element: ini_list) 
        push_back(element);
      }

  
    //Copy-Konstruktor using Deep-Copy semantics (Aufgabe 3.5)
    List(List<T> const& c):
      size_ {0},
      first_ {nullptr},
      last_ {nullptr}
    { 
        ListNode<T>* it = c.first_; 
        for(int i = 0; i < c.size(); ++i){ 
          push_back(it->value);
          it = it->next;
        }
    }

    //(unifying) Assignment operator (Aufgabe 3.6)
    /* Ersetzt den Inhalt der Liste mit einer Kopie der Inhalte der uebergebenen Liste. */
    List<T>& operator=(List<T> rhs) { // per-value Uebergabe, copy constructor aufgerufen (move constructor, falls temporäres Objekt)
      swap(rhs);
      return *this;
    } //Destruktor fuer rhs-Kopie aufgerufen

    /* Tauscht die Inhalte der Liste mit der übergebenen, indem first und last Pointer zwischen den Listen vertauscht werden */
    // needed for 3.6 unifying assignment operator
    void swap(List<T>& rhs) { 
      ListNode<T>* firsttemp = rhs.first_;
      ListNode<T>* lasttemp = rhs.last_;
      std::size_t sizetemp = rhs.size_;
      rhs.first_ = first_;
      rhs.last_ = last_;
      rhs.size_ = size_;
      first_ = firsttemp;
      last_ = lasttemp;
      size_ = sizetemp;
    }

    /* operator== (Aufgabe 3.8)
    Checks if the contents of lhs and rhs are equal
    (if they have the same number of elements and each element in lhs compares equal with the element in rhs at the same position.*/
    bool operator==(List const& rhs) const {
      if(size_ != rhs.size_){
        return false;
      }
      ListNode<T>* it = first_; 
      ListNode<T>* rhsit = rhs.first_; 
      for(int i = 0; i < size(); ++i){ 
        if(it->value != rhsit->value){
          return false;
        }
        it = it->next;
        rhsit = rhsit -> next;
      }
      return true;
    }

    /* operator!= (Aufgabe 3.8) */
    bool operator!=(List const& rhs) const{
      if (operator==(rhs)){
        return false;
      }
      else{
        return true;
      }
    }

    /* Destruktor */
    ~List() {
      clear(); 
    }

    //begin-Method  (Aufgabe 3.10)
    /* returns an Iterator to the first element in the List */
    ListIterator<T> begin(){
      return ListIterator<T>{first_};
    }

    /* returns an Iterator to the element after the last element in the List, therefore a nullptr */
    ListIterator<T> end(){
      return ListIterator<T>{nullptr};
    }


    /* inserts value before pos
    @param 	iterator before which the content will be inserted (can also be the end() iterator)
    @param 	element value to insert
    @return iterator to the inserted element */
    //TODO: member function insert (Aufgabe 3.12)
    ListIterator<T> insert(ListIterator<T> const& pos, T const& element){
      
      if(pos.node == nullptr && pos != end()){ 
        throw "Iterator does not point to valid node";
      }
      else if(pos == begin()){
        push_front(element);
        return begin();
      }
      else if(pos == end()){
        push_back(element);
        return end();
      }
      else{
        ListNode<T> *n = new ListNode<T> {element, pos.node->prev, pos.node};
        pos.node->prev->next = n;
        pos.node->prev = n;
        ++size_;
        return ListIterator<T>{n};
      }
    }


    /*  Removes the element at pos.

    @param 	iterator to the element to remove 
    (The iterator pos must be valid and dereferenceable. 
    Thus the end() iterator (which is valid, but is not dereferencable) 
    cannot be used as a value for pos.)

    @return  Iterator following the removed element.
    If pos refers to the last element, then the end() iterator is returned.*/
    ListIterator<T> erase(ListIterator<T> const& pos){
      if(empty()){  
        throw "List is empty";
      }
      if(pos.node == nullptr){
        throw "Iterator to the element to remove is a nullpointer.";
      }
      else if(pos == begin()){ 
        pop_front();
        return begin();
      }
      else if (pos == ListIterator<T>{last_}){ //pos is the last element
        pop_back();
        return end();
      }
      else{
        auto temp = pos;
        auto returnvalue = ++temp;
        pos.node->prev->next = pos.node->next;
        pos.node->next->prev = pos.node->prev;
        assert(nullptr != pos.node);
        delete pos.node;
        --size_;
        return ListIterator<T>{returnvalue};
      }
    }




    /* Reverses the order of the elements in the list (Aufgabe 3.7 - Teil 1) */
    void reverse(){
      ListNode<T>* it = first_; 
      for(int i = 0; i < size(); ++i){ 
        ListNode<T>* tempnext = it->next; 
        it->next = it->prev;
        it->prev = tempnext;
        it = it->prev;
      }
      ListNode<T>* tempfirst = first_;
      first_ = last_;
      last_ = tempfirst;
    }


    /* element added at the front of the list */
    void push_front(T const& element) {
      ListNode<T> *n = new ListNode<T> {element, nullptr, nullptr};
      if(empty()){
        first_ = n;
        last_ = n;
      }
      else{
        n->next = first_;
        first_->prev = n;
        first_=n;
      }
      ++size_;
    }

    /*element added at the end of the list */
      void push_back(T const& element) {
      ListNode<T> *n = new ListNode<T> {element, nullptr, nullptr};
      if(empty()){
        first_ = n;
        last_ = n;
      }
      else{
        n->prev = last_;
        last_->next = n;
        last_ = n;
      }
      ++size_;
    }

    /* remove an element from the front of the list (without returning it) */
    void pop_front() {
      ListNode<T>* it = first_;
      if(empty()) {
        throw "List is empty";
      }
      else if(size()==1){
        first_ = nullptr;
        last_ = nullptr;
        assert(nullptr != it);
        delete it; 
        --size_;
      }
      else{
        first_ = first_->next;
        first_->prev = nullptr;
        assert(nullptr != it);
        delete it;
        --size_;
      }
    }

    /* remove an element from the end of the list (without returning it) */
    void pop_back() {
      ListNode<T>* it = last_;
      if(empty()) {
        throw "List is empty";
      }
      else if(size()==1){
        first_ = nullptr;
        last_ = nullptr;
        assert(nullptr != it);
        delete it;
        --size_;
      }
      else{
        last_= last_->prev;
        last_->next = nullptr;
        assert(nullptr != it);
        delete it;
        --size_;
      }
    }

    /* Returns a reference to the first element in the container. */
    T& front() {
      if(empty()) {
        throw "List is empty";
      }
      else{
        return first_->value;
      }
    }

    /* Returns a reference to the last element in the container. */
    T& back() {
      if(empty()) {
        throw "List is empty";
      }
      else{
        return last_->value;
      }
    }

    /* returns whether the list is empty */
    bool empty() const {
      if(size() == 0){
        return true;
      }
      return false;
    };


    /* returns the size of the list (how many elements there are stored in the list) */
    std::size_t size() const{ //std::size_t is the unsigned integer type of the result of the sizeof operator    
      return size_;
    };

    /* deletes all elements in the list*/
    void clear(){
      while(empty() == false){
        pop_front();
      }
    }


  // list members
  private: 
    std::size_t size_;
    ListNode<T>* first_;
    ListNode<T>* last_;
    int move_aufrufe = 0;
};

/* Reverses the order of the elements of a list 
@param ListNode-Liste 
@return new List in reversed order */
//(Aufgabe 3.7 - Teil 2, benutzt Member-Funktion reverse)
template <typename T>
List<T> reverse(List<T> const& l){ 
  List<T> reverselist = l;
  reverselist.reverse();
  return reverselist;
}

/* Zwei Listen des selben Typs werden zu einer einzigen zusammengefuegt, sodass die Reihenfolge der Elemente
gleich bleibt (erst Elemente der ersten Liste, dann Elemente der zweiten).*/
//Freie Funktion operator+ (3.14 - Teil 2)
template <typename T>
List<T> operator+(List<T>& first, List<T>& second){ //keine const-Parameteruebergabe wegen begin() und end()
  List<int> r{first};
  for (T& element: second){ 
    r.push_back(element);
  }
  return r;
}


#endif // # define BUW_LIST_HPP
