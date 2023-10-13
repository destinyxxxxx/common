#pragma once

template <typename T>
class singleton
{
public:
    static T& instance()
    {
        // function-local static to force this to work correctly at static
        // initialization time.
        static T instance;
        return instance;
    }

    singleton(const singleton&) = delete;
    singleton& operator=(const singleton&) = delete;

protected:
    singleton() = default;
    virtual ~singleton() = default;
};

/******************************************************************************/

template <typename T>
class singleton_default
{
private:
    struct object_creator
    {
      // This constructor does nothing more than ensure that instance()
      //  is called before main() begins, thus creating the static
      //  T object before multithreading race issues can come up.
      object_creator() { singleton_default<T>::instance(); }
      inline void do_nothing() const { }
    };
    static object_creator create_object;

protected:
    singleton_default() = default;
    virtual ~singleton_default() = default;

public:
    typedef T object_type;

    // If, at any point (in user code), singleton_default<T>::instance()
    //  is called, then the following function is instantiated.
    static object_type& instance()
    {
      // This is the object that we return a reference to.
      // It is guaranteed to be created before main() begins because of
      //  the next line.
      static object_type obj;

      // The following line does nothing else than force the instantiation
      //  of singleton_default<T>::create_object, whose constructor is
      //  called before main() begins.
      create_object.do_nothing();

      return obj;
    }

    singleton_default(const singleton_default&) = delete;
    singleton_default& operator=(const singleton_default&) = delete;
};
template <typename T>
typename singleton_default<T>::object_creator
singleton_default<T>::create_object;