#include "binarynet.h"
#include <assert.h>
#include <iostream>
#include <bitset>

//====================================================================================//
// CLASS BINARY_NET
//====================================================================================//
BinaryNet::BinaryNet(vector<uint32_t> _topology) {
    unsigned num_layers = _topology.size();

    for (unsigned layer_num = 0; layer_num < num_layers; ++layer_num) {
        layers.push_back(Layer());

        // The output layer does not have any connection
        uint32_t num_outputs = layer_num == _topology.size()-1 ? 0 : _topology[layer_num+1];
        
        for (uint32_t neuron_num = 0; neuron_num <= _topology[layer_num]; ++neuron_num) {
            layers.back().push_back(Neuron(num_outputs, neuron_num));
        }
        
        // Force the bias node's output to 1.0 (it is the last neuron pushed in this layer):
        layers.back().back().set_output(1.0);
    }
}

uint32_t BinaryNet::predict(vector<int> &input) {
    // Forward pass
    feed_forward(input);

    // Get raw outputs
    vector<int> model_output = get_result();
    
    // Apply argmax to the output to get the highest probable class
    int predicted_class = BinaryNet::argmax(model_output);

    num_preds++;
    if (num_preds % 100000 == 0) {
        cout << "num_preds = " << num_preds << endl;
    }

    return predicted_class;
}

void BinaryNet::feed_forward(vector<int> &input) {
    // Sanity check: # of input must equal to # of input neurons
    assert(input.size() == layers[0].size()-1);

    // [Input Layer] Assign input values into the input neurons
    for (unsigned i=0; i < input.size(); ++i) {
        layers[0][i].set_output(input[i]);
    }

    // [Hidden Layers] Forward propagate
    for (unsigned layer_num = 1; layer_num < layers.size()-1; ++layer_num) {
        Layer &prev_layer = layers[layer_num-1];
        for (unsigned n=0; n < layers[layer_num].size(); ++n) {
            layers[layer_num][n].feed_forward(prev_layer, true);
        }
    }

    // [Output Layer] Forward propagate, but do not apply neuron activation
    Layer &prev_layer = layers[layers.size()-2];
    for (unsigned n=0; n < layers.back().size(); ++n) {
        layers.back()[n].feed_forward(prev_layer, false);
    }
}

vector<int> BinaryNet::get_result() {
    vector<int> outputs = {};

    // Return the output of the output neurons
    for (unsigned i=0; i < layers.back().size()-1; ++i)
        outputs.push_back(layers.back()[i].get_output());
    
    return outputs;
}

int BinaryNet::argmax(vector<int> &input) {
    int argmax = 0;
    for (unsigned i=0; i < input.size(); ++i) {
        if (input[i] > input[argmax])
            argmax = i;
    }
    return argmax;
}

void BinaryNet::init_weights(vector<vector<int>> &weights) {
    // Sanity check: # of layers must be consistent 
    assert((layers.size()-1) == weights.size());

    // Sanity check: # of weights per layer must be consistent with # of neurons
    for (unsigned i=0; i < layers.size()-1; ++i)
        assert(layers[i].size()*(layers[i+1].size()-1) == weights[i].size());

    // Assign the input weight to each connection
    for (unsigned i=0; i < layers.size()-1; ++i) {
        for (unsigned n=0; n < layers[i].size(); ++n) {
            int num_connections = layers[i+1].size();
            for (int j=0; j < num_connections; ++j)
                layers[i][n].out_weights[j] = weights[i][n*num_connections+j];
        }
    }
}

vector<int> BinaryNet::binarize_addr(uint64_t address) {
    string binary = std::bitset<58>(address).to_string();
    vector<int> binarized_addr;
    for (unsigned i=0; i < binary.size(); ++i) {
        if (binary[i] == '0')
            binarized_addr.push_back(0);
        else
            binarized_addr.push_back(1);
    } 
    return binarized_addr;
}

//====================================================================================//
// CLASS NEURON
//====================================================================================//
Neuron::Neuron(int _num_outs, int _index) {
    my_index = _index;
    for (int i=0; i < _num_outs; ++i)
        out_weights.push_back(0);
}

void Neuron::feed_forward(Layer &prev_layer, bool activate) {
    int sum = 0;

    // Sum outputs of neuron in the previous layer
    for (unsigned i=0; i < prev_layer.size(); ++i) {
        sum += prev_layer[i].get_output() * prev_layer[i].out_weights[my_index];
    }

    // Apply activation function on the sum
    if (activate)
        this->out_val = Neuron::sign(sum);
    else
        this->out_val = sum;
}

int Neuron::sign(int input) {
    if (input < 0)
        return -1;
    else
        return 1;
}

// 