#include <iterator>
#include <glm/glm.hpp>

class IVec3RangeIterator
{
private:
    glm::ivec3 start_;
    glm::ivec3 end_;
    class ivec3Holder
    {
        glm::ivec3 value_;
    public:
        ivec3Holder(const glm::ivec3& value): value_(value) {}
        glm::ivec3 operator*() { return value_; }
    };
public:
    // Previously provided by std::iterator - see update below
    typedef glm::ivec3              value_type;
    typedef glm::ivec3              difference_type;
    typedef glm::ivec3*             pointer;
    typedef glm::ive3&              reference;
    typedef std::input_iterator_tag iterator_category;

    explicit IVec3RangeIterator(glm::ivec3 start, glm::ivec3 end) 
        : start_(start) 
        , end_(end)
    { }

    glm::ivec3 operator*() const { return value_; }
    bool operator==(const IVec3RangeIterator& other) const { 
        return start_ == other.start_ && end_ == other.end_;
    }
    bool operator!=(const IVec3RangeIterator& other) const { 
        return !(*this == other); 
    }

    /*
    ivec3Holder operator++()
    {
        ivec3Holder ret(start_);
        ++*this;
        return ret;
    }
    */

    IVec3RangeIterator& operator++()
    {
        
        return *this;
    }
};
