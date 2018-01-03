
#pragma once

#include <cstdlib>
#include <vector>
#include <utility>

namespace audioelectric {

  /*!\brief Conatins and manages a set of audio data. Useful for samples, grains or any other chunk of audio data that needs
   * needs to be stored and manipulated.
   */
  template<typename DType>
  class Waveform : public std::vector<DType>{
  public:
    enum InterpType {
      LINEAR,
    };

    /*!\brief An iterator that generalizes element access to allow for interpolation between elements
     */
    template<typename T>
    class iterator {
    public:
      iterator(std::unique_ptr<Waveform<T>> wf, double speed=1);
      iterator<T>& operator++(void);    //!<\brief Prefix increment
      iterator<T>& operator++(int);     //!<\brief Postfix increment
      iterator<T> operator+(int n);     //!<\brief Random access +
      iterator<T> operator-(int n);     //!<\brief Random access -
      T& operator*(void);
      T* operator->(void);
      bool operator==(const iterator<T>& other) const;
      bool operator!=(const iterator<T>& other) const;
    private:
      double _speed;
      std::unique_ptr<Waveform<T>> _wf;
    };

    Waveform(std::initializer_list<DType> init, InterpType=LINEAR);
    
    /*!\brief Copies a sample to a new length using interpolation
     */
    Waveform(Waveform<DType> other, std::size_t len, InterpType=LINEAR);

    void setInterpType(InterpType it) {_interptype = it;}

    InterpType getInterpType(void) const {return _interptype;}

    /*!\brief Returns the interpolated value at a position in the waveform.
     * 
     * This is a generalized form of the integer-related operator[], in which points between the integer positions are 
     * interpolated.
     * 
     * \param pos The position on the waveform
     * \return The interpolated value at pos
     */
    const DType& operator[](double pos) const;

    /*!\brief Returns the interpolated value at a position in the waveform.
     * 
     * This is a generalized form of the integer-related operator[], in which points between the integer positions are 
     * interpolated.
     * 
     * \param pos The position on the waveform
     * \return The interpolated value at pos
     */
    DType& operator[](double pos);

    iterator<DType> begin(double speed=1);
    iterator<DType> end(void);
    
  private:

    InterpType _interptype;
  
  };

}
