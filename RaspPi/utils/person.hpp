#ifndef PERSON_HPP_INCLUDED
#define PERSON_HPP_INCLUDED

class Person
{

public:
    int x,y,cnt;
    bool found = true;

    /**
    * Constructor
    */
    Person(int centerX,
            int centerY);


    /**
    * Increment cnt if person fnd
    */
    void increment_cnt();

    /**
    * Increment cnt if person fnd
    */
    void decerement_cnt();

    /**
    * Update the persons pose if they are moving
    */
    void update_pose(int new_x, int new_y);

protected:

private:
};


#endif // PERSON_HPP_INCLUDED
