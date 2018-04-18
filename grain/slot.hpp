
#pragma once
#include <functional>
#include <utility>
#include <set>

namespace audioelectric {

  namespace ph = std::placeholders;
  
  /*! Generates an a function pointer and a hash from an object and method that match the FnBang signature.
   *  Because you can't cast a function method to a void*, the method part is hashed using the string name of the
   *  method. 
   * 
   * \param FN A pointer to the function (eg, &MyClass::myFunction)
   * \param OBJ A pointer to the object (eg, &myobj, or this)
   */
#define MakeSlot(FN, OBJ) Slot(Slot::FnDbl(std::bind(FN, OBJ)), \
                               Slot::Hash((void*)OBJ,                   \
                                          (void*)std::hash<std::string>{}(std::string(#FN))))

  /*!\brief Provides an interface for a generic callback. 
   * 
   * Wraps a function with any one of a selection of signatures and allows it to be called with any one of another selection
   * of signatures, thereby allowing objects that may talk with different interfaces to communicate with each other.
   */
  class Slot {
  public:
    
    /*! A function for tics (receives a tic result as an argument)
     */
    using FnDbl = std::function<void(double)>;

    /*! A pair that hashes to a method and its object. In the case that it's a bare function, the object part of the hash is
     *  0x0. This is used to compare Slots.
     */
    using Hash = std::pair<void*,void*>;  
    
    Slot(void) : _fn(), _hash(0,0) {}
    Slot(const Slot& slot);
    ~Slot(void) {}

    /*!\brief Constructs from a FnDbl function. This should generally be called using the SLOT_TIC macro.
     */
    Slot(const FnDbl& fn, Hash hash);

    /*!\brief Constructs from a function pointer
     */
    Slot(void(*fn)(double));

    /*!\brief Casts to true if the slot has a valid function. Otherwise casts to false
     */
    operator bool(void) const {return (bool)_fn;}

    /*!\brief Casts to false if the slot has a valid function. Otherwise casts to true
     */
    bool operator!(void) const {return !_fn;}

    Slot& operator=(const Slot& rhs);
    Slot& operator=(void(*rhs)(double));
    bool operator==(const Slot& rhs) const {return _hash == rhs._hash;}
    bool operator!=(const Slot& rhs) const {return _hash != rhs._hash;}
    bool operator<(const Slot& rhs) const {return _hash < rhs._hash;}
    bool operator>(const Slot& rhs) const {return _hash > rhs._hash;}
    bool operator<=(const Slot& rhs) const {return _hash <= rhs._hash;}
    bool operator>=(const Slot& rhs) const {return _hash >= rhs._hash;}

    /*!\brief Calls the wrapped function.
     * 
     * If there is no function (the (bool)(*this) == false) then nothing is called.
     * 
     * \param arg The argument of the function
     */
    void operator()(double arg=0) const;
    
  private:
    Hash _hash;
    FnDbl _fn;
  };

  using Signal = std::set<Slot>;  
  
}
