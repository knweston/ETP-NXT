#ifndef BINARY_NET_H
#define BINARY_NET_H

#include <champsim.h>
#include <vector>

using namespace std;

//====================================================================================//
// FORWARD DECLARATIONS
//====================================================================================//
class Neuron;
typedef vector<Neuron> Layer;


//====================================================================================//
// CLASS BINARY_NET
//====================================================================================//
class BinaryNet {
public:
    BinaryNet(vector<uint32_t> _topology);

    /* 
        Input is organized as follows:
        - One weight vector per layer (except the output layer)
        - Within a weight vector, one weight per connection
    */
    void init_weights(vector<vector<int>> &weights);

    /*
        - Input data must be converted to integers
        - Returns a prediction of type integer
    */
    uint32_t predict(vector<int> &input);
    
    /*
        - Proceeds a forward pass through the hidden layers
        using the input data given in the parameter
        - Sign(x) activation for all hidden layers
        - No activation for the output layer
    */
    void feed_forward(vector<int> &input);

    /*
        - Collect output from each output neuron
    */
    vector<int> get_result();

    static vector<int> binarize_addr(uint64_t address);

private:
    vector<Layer> layers;
    uint64_t num_preds = 0;
    /*
        Classification activation function (for output layer)
    */
    static int argmax(vector<int> &input);
};


//====================================================================================//
// CLASS NEURON
//====================================================================================//
class Neuron {
public:
    Neuron(int _num_outs, int _index);
    
    void feed_forward(Layer &prev_layer, bool activate);
    void set_output(int val) { out_val = val; }
    int  get_output() { return out_val; }

private:
    int out_val;
    int my_index;
    vector<int> out_weights;
    
    // BNN activation function = sign(x)
    static int sign(int input);

    friend class BinaryNet;
};

#endif // BINARY_NET_H