float vertices[6][12] = {
{
   -1, 1,-1,
    1, 1,-1,
    1,-1,-1,
   -1,-1,-1,// Front face
},{
    1, 1, 1,
   -1, 1, 1,
   -1,-1, 1,
    1,-1, 1, // Back face
},{
   -1, 1, 1,
    1, 1, 1,
    1, 1,-1,
   -1, 1,-1,   // Up face
},{
   -1,-1,-1,
    1,-1,-1,
    1,-1, 1,
   -1,-1, 1, // Down face
},{
   -1, 1, 1,
   -1, 1,-1,
   -1,-1,-1,
   -1,-1, 1, // Left face
},{
    1, 1,-1,
    1, 1, 1,
    1,-1, 1,
    1,-1,-1,// Right face
}
};