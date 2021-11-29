/*
  ==============================================================================

    Fifo.h
    Created: 16 Oct 2021
    Author:  Thomas Boggs   

  ==============================================================================
*/

#pragma once
#include <vector>
#include "../JuceLibraryCode/JuceHeader.h"


//=============================================================================
/*
 Template Specialization To check if T is ReferenceCountedObjectPtr
 */

template <typename T>
struct IsReferenceCountedObjectPtr : std::false_type { };

template <typename T>
struct IsReferenceCountedObjectPtr<juce::ReferenceCountedObjectPtr<T>> : std::true_type { };

template <typename T>
struct IsReferenceCountedArray : std::false_type { };

template <typename T>
struct IsReferenceCountedArray<juce::ReferenceCountedArray<T>> : std::true_type { };

// Check For Vector
template <typename T>
struct IsVector : std::false_type { };

template <typename T, typename A>
struct IsVector<std::vector<T, A>> : std::true_type { };

// Check For AudioBuffer
template <typename T>
struct IsAudioBuffer : std::false_type { };

template <typename T>
struct IsAudioBuffer<juce::AudioBuffer<T>> : std::true_type { };


//=============================================================================
/*
 Fifo
 */
template <typename T, size_t Size>
struct Fifo
{
    Fifo () : bufferSize(Size) {}
    
    size_t getSize() { return bufferSize; }
    
    //used when T is AudioBuffer<float>
    void prepare(int numSamples, int numChannels)
    {
        static_assert(std::is_same_v<T, juce::AudioBuffer<float>>, "Fifo Template Type should be juce::AudioBuffer<float> if using prepare with two arguments.");
        
        /*
         Steps to Prepare for AudioBuffer
         - Loop Through buffer
         - Initialize each audioBuffer to correct size
         - Clear Data inside each buffer in the array of buffers
         */
        for (auto& bufferItem : buffer)
        {
            bufferItem.setSize(numChannels,
                           numSamples,
                           false,
                           true,
                           true);
            bufferItem.clear();
        }
    }
    
    //used when T is std::vector<float>
    void prepare(size_t numElements)
    {
        static_assert(std::is_same_v<T, std::vector<float>>, "Fifo Template Type should be std::vector<float> if using prepare with one input argument.");
        
        /*
         Steps to prepare std::vector<float> buffertype
         - Loop through all the buffers in buffer
         - Set the vector to the correct size
         - initialize all data to 0's
         */
        for (auto& bufferItem : buffer)
        {
            bufferItem.clear(); // Leaves vector with size=0
            bufferItem.setSize(numElements, 0); // set vector to size numElements and init to 0
        }
    }
    
    /*
     If T is a ref-counted object:
     - is_same_v?? todo
     - Make sure that refcount doesn't go to 0
     
     Otherwise, push as normal.
     */
    bool push(const T& t)
    {
        auto writeHandle = fifo.write (1);
        
        if (writeHandle.blockSize1 > 0)
        {
            // Cast the writeindex to size_t, because the return type is int
            size_t index = static_cast<size_t>(writeHandle.startIndex1);
            
            if constexpr (IsReferenceCountedObjectPtr<T>::value)
            {
                auto currentBuf = buffer[index];
                jassert(currentBuf.get() == nullptr || currentBuf->getReferenceCount() > 1);
                
                buffer[index] = t;
            }
            else
            {
                buffer[index] = t;
            }
            return true;
        }
        
        return false;
    }
    
    bool pull(T& t)
    {
        auto readHandle = fifo.read(1);
        if (readHandle.blockSize1 > 0)
        {
            // Cast the readindex to size_t, because the return type is int
            size_t index = static_cast<size_t>(readHandle.startIndex1);
            t = buffer[index];
            return true;
        }
        return false;
    }
    
    int getNumAvailableForReading() const
    {
        return fifo.getNumReady();
    }
    
    int getAvailableSpace() const
    {
        return fifo.getFreeSpace();
    }
    
    bool exchange(T&& t)
    {
        auto readHandle = fifo.read(1);
        if (readHandle.blockSize1 > 0)
        {
            // Cast the readindex to size_t, because the return type is int
            size_t index = static_cast<size_t>(readHandle.startIndex1);
            
            /*
             if T is a reference counted object pointer
                 swap t with buffer[idx]
                 make sure buffer[idx] now holds a nullptr
             else
                if T is a vector
                    if t's size is < buffer[idx]'s size,
                         you can't swap
                         you need to copy
                    else t's size is >= buffer[idx]'s size, which means:
                         you CAN swap.
                else
                    if T is a juce::AudioBuffer
                         if t's size is < buffer[idx]'s size,
                            you can't swap
                            you need to copy
                         else t's size is >= buffer[idx]'s size, which means:
                            you CAN swap.
                    else
                         blindly swap.
                         and maybe jassert just so you can investigate
                         if any Ts actually lead to this code path
                         and determine if you should swap or copy.
             */
            
            if constexpr (IsReferenceCountedObjectPtr<T>::value)
            {
                std::swap(buffer[index], t);
                jassert (buffer[index] == nullptr);
            }
            else
            {
                if constexpr (IsReferenceCountedArray<T>::value)
                {
                    std::swap(buffer[index], t);
                    jassert (buffer[index].size() == 0);
                }
                else
                {
                    if constexpr (IsVector<T>::value)
                    {
                        if (t.size() < buffer[index].size())
                        {
                            // Need to Copy
                            t = buffer[index];
                        }
                        else
                        {
                            // Can Swap
                            std::swap(buffer[index], t);
                        }
                    }
                    else
                    {
                        if constexpr (IsAudioBuffer<T>::value)
                        {
                            if (t.getNumSamples() < buffer[index].getNumSamples())
                            {
                                // Need to Copy
                                t = buffer[index];
                            }
                            else
                            {
                                // Can Swap
                                std::swap(buffer[index], t);
                            }
                        }
                        else
                        {
                            // Blindly Swap
                            std::swap(buffer[index], t);
                            jassertfalse; // To see if anything ends up here
                        }
                    }
                }
                
            }
            
            return true;
        }
        return false;
    }

private:
    juce::AbstractFifo fifo { Size };
    std::array<T, Size> buffer;
    size_t bufferSize;
};


