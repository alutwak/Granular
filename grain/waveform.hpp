
#pragma once

#include <cstdlib>
#include <initializer_list>

namespace audioelectric {

  /*!\brief Different types of interpolation
   */
  enum class InterpType {
    LINEAR,     //!< Linear interpolation
  };

  /*!\brief Contains and manages a set of audio data. Useful for samples, grains or any other chunk of audio data that needs
   * needs to be stored and manipulated.
   */
  template<typename T>
  class Waveform final {
  public:

    /*!\brief A common iterator for the Waveform. This works just like that of a vector's iterator
     */
    class iterator {
    public:
      iterator(T* data) : _data(data) {};
      iterator(const iterator& other) : _data(other._data) {};
      iterator& operator++(void);
      iterator operator++(int);
      T& operator*(void);
      bool operator==(const iterator& other) const;
      bool operator!=(const iterator& other) const;
    private:
      T* _data;
    };

    /*!\brief Creates a waveform of size 0
     */
    Waveform(void);

    /*!\brief Creates and allocates a waveform of size len with all values set to 0
     */
    Waveform(std::size_t len, InterpType it=InterpType::LINEAR);

    /*!\brief Wraps an array of size len in a Waveform to allow it to be interpolated
     */
    Waveform(T* data, std::size_t len, InterpType it=InterpType::LINEAR);

    Waveform(std::initializer_list<T> init, InterpType it=InterpType::LINEAR);

    /*!\brief Creates a new Waveform and fills it with a generator function
     *
     * \see generate()
     *
     * \param generator The generator function to use
     * \param len The length of the waveform to generate
     */
    Waveform(T (*generator)(size_t), size_t len, InterpType it=InterpType::LINEAR);

    /*!\brief Copies a sample to a new length using interpolation
     */
    Waveform(Waveform<T>& other, double rate, std::size_t len, InterpType it=InterpType::LINEAR);

    ~Waveform(void);

    /*!\brief Generates the waveform with a generator function.
     *
     * The generator function takes an index and generates a value at that index. generate() will use the generator function
     * to fill the waveform data by calling the generator for each integral number from 0 to len-1.  For instance, the
     * following generator would generate a sin function with one cycle over 44100 samples:
     *
     * \code{.cpp}
     * wf.generate([](size_t i) {return sin(2.*PI*i/44100.);}, 44100);
     * \endcode
     *
     * Note, that if the waveform already has data in it, it will be resized using the resize() function before calling the
     * generator.
     *
     * \param generator The generator function to use
     * \param len The length of the waveform to generate
     */
    void generate(T (*generator)(size_t), size_t len);

    /*!\brief Sets the interpolation type
     */
    void setInterpType(InterpType it) {_interptype = it;}

    /*!\brief Returns the current interpolation type
     */
    InterpType getInterpType(void) const {return _interptype;}

    /*!\brief Returns the interpolated value at a position in the waveform.
     *
     * The position is a generalized index of the samples in the waveform, such that an integer position will return
     * the same value as that returned by Waveform[], and a non-integer position will return an interpolated value as though
     * the waveform were continuous.
     *
     * Positions outside of the waveform -- those less than 0 and greater than the size of the waveform (in samples) -- will
     * return a value of 0. This is done so that a Waveform can be easily mixed with other Waveforms without having to worry
     * about bounds.
     *
     * \param pos The position on the waveform
     * \param channel The channel to retrieve from (ignored for now)
     * \return The interpolated value at pos
     */
    T waveform(double pos, int channel=0);

    Waveform<T>& operator=(const Waveform<T>& other);

    T& operator[](std::size_t pos) {return _data[pos];}
    const T& operator[](std::size_t pos) const {return _data[pos];}

    iterator ibegin(void);
    iterator iend(void);

    /*!\brief Returns the number of samples in the Waveform
     */
    std::size_t size(void) const {return _size;}

    /*!\brief Returns the end position of the Waveform
     */
    double end(void) const {return _size-1;}

    /*!\brief Resizes the Waveform. All data is cleared
     */
    void resize(std::size_t len);

    /*!\brief Returns a pointer to the raw data
     */
    T* data(void) {return _data;}

  private:

    InterpType _interptype;     //!< The interpolation type
    size_t _size;               //!< The size of data
    size_t _end;                //!< The last index of _data
    T* _data;                   //!< The raw data

    /*!\brief Allocates a data array of length len
     */
    void alloc(std::size_t len);

    /*!\brief Deallocates _data
     */
    void dealloc(void);

    /*!\brief Performs a linear interpolation of the point at pos
     *
     * If pos is <0 or >_size-1, this will always return 0
     */
    T interpLinear(double pos) const;

  };

  /*!\brief Generates a gaussian function and puts it in a wavetable
   *
   * The generated function is: r*(e^(-(x-len/2)^2/sigma)-y), where y is the value of the normal Gaussian at the
   * beginning and end of the signal and r renormalizes the signal after that subtraction. This ensures that the signal
   * begins and ends with a value of zero.
   *
   * \param[in,out] wf The Wavetable to fill (this will overwrite any function already in it)
   * \param len        The length of the gaussian function
   * \param sigma      The variance of the gaussian. Useful values are on the range of [0,1]
   */
  template<typename T>
  void GenerateGaussian(Waveform<T>& wf, std::size_t len, T sigma);

  /*!\brief Generates one cycle of a sine wave
   * 
   * \param[in,out] wf The Wavetable to fill (this will overwrite any function already in it)
   * \param len        The length of the gaussian function
   */
  template <typename T>
  void GenerateSin(Waveform<T>& wf, std::size_t len);

  /*!\brief Generates one cycle of a triangle wave
   * 
   * \param[in,out] wf The Wavetable to fill (this will overwrite any function already in it)
   * \param len        The length of the gaussian function
   * \param slant      The anount of slant to the triangle. A value of 0 gives equal time to the rise and the fall. A value 
   *                   of 1 results in all rise.
   */
  template <typename T>
  void GenerateTriangle(Waveform<T>& wf, std::size_t len, T slant);

  /*!\brief Generates one cycle of a square wave
   * 
   * \param[in,out] wf The Wavetable to fill (this will overwrite any function already in it)
   * \param len        The length of the gaussian function
   * \param width      The width of the square. A value of 0 results in a silent signal (all zeros). A value of 1 results in 
   *                   all ones. Useful values are between those extremes.
   */
  template <typename T>
  void GenerateSquare(Waveform<T>& wf, std::size_t len, T width);

}
