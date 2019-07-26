
/**
 This class draws a table by scaling and moving blocks.
 The table top is the plane y = 0
*/

class Table
{
    float tableX, tableY, tableZ;
    float legX, legY, legZ;
    
public:
    Table( float width, float length, int id );
    void render(int programID);
};
