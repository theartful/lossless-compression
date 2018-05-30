#include "ppm_wizard.h"

typedef util::Symbol Symbol;
typedef util::Node Node;

PPMModel::PPMModel()
{
    context = new Buffer(MAX_CONTEXT, false);
    trie = new TrieNode();

    for(int i = 0; i < F; i++)
    {
        for(int j = 0; j < D; j++)
        {
            alpha_matrix[i][j] = 0.5;
            beta_matrix[i][j] = 0.85;
        }
    }

    for(int i = 0; i < NUMBER_OF_SYMBOLS; i++)
    {
        symbolProbabilities[i] = -1;
    }

    currentContext = 0;
}

int PPMModel::getSymbolLength()
{
    return SYMBOL_LENGTH;
}

int PPMModel::getEof()
{
    return EOF_SYMBOL;
}

void PPMModel::addSymbol(int symbol)
{
    //updateCoefficients(symbol);
    currentContext = context->getAbsoluteSize() > MAX_CONTEXT ? MAX_CONTEXT : context->getAbsoluteSize();

    updateFrequencies(symbol);
    context->addByte(symbol);

    for(int i = 0; i < NUMBER_OF_SYMBOLS; i++)
    {
        symbolProbabilities[i] = -1;
    }
}

Model::ull PPMModel::getTotCount()
{
    return DENOMINATOR + NUMBER_OF_SYMBOLS * MINIMUM_PROP;
}

Model::ull PPMModel::getCumCount(int k)
{
    ull cum_prop = 0;
    for(int i = 0; i <= k; i++)
    {
        cum_prop += getProbability(i);
    }
    return cum_prop;
}

int PPMModel::getNumberOfSymbols()
{
    return NUMBER_OF_SYMBOLS;
}

void PPMModel::compressSymbol(int byte, ull& sym_low, ull& sym_high, ull& tot_count)
{
    sym_low = getCumCount(byte - 1);
    sym_high = getCumCount(byte);
    tot_count = getTotCount();
}

void PPMModel::updateFrequencies(int byte)
{
    bool last = false;
    for(int i = currentContext; i >= 0; i--)
    {
        TrieNode* node = getTrieNode(i, true);
        if(node->trieNodes[byte] == nullptr)
        {
            node->trieNodes[byte] = new TrieNode();
        }
        else
        {
            last = true;
        }
        node->trieNodes[byte]->frequency++;
        node->cum_count++;
        if(last) break;
    }
}

int PPMModel::getFanout(TrieNode* trieNode)
{
    int fanout = 0;
    for(int i = 0; i < NUMBER_OF_SYMBOLS; i++)
    {
        if(trieNode->trieNodes[i] != nullptr)
            fanout++;
    }
    return fanout;
}

double PPMModel::_getProbability(int depth, int k)
{
    if(depth == -1)
    {
        return (1.0d / NUMBER_OF_SYMBOLS);
    }

    TrieNode* node = getTrieNode(depth, false);

    int fanout = getFanout(node);

    int i = fanout >= F? F - 1 : fanout;
    int j = depth >= D? D - 1 : depth;
    double alpha = alpha_matrix[i][j];
    double beta = beta_matrix[i][j];

    double firstTerm, secondTerm;
    if(node->trieNodes[k] == nullptr)
        firstTerm = 0;
    else
        firstTerm = node->trieNodes[k]->frequency - beta;
    secondTerm = (fanout * beta + alpha) * _getProbability(depth - 1, k);

    contextProbabilities[depth] = (firstTerm + secondTerm) / (node->cum_count + alpha);
    return contextProbabilities[depth];
}

Model::ull PPMModel::getProbability(int k)
{
    if(symbolProbabilities[k] != -1)
        return symbolProbabilities[k];

    double p = _getProbability(currentContext, k);
    ull prop = p * DENOMINATOR;

    if(prop < MINIMUM_PROP)
    {
        prop = MINIMUM_PROP;
    }

    symbolProbabilities[k] = prop;
    return prop;
}

void PPMModel::updateCoefficients(int symbol)
{
    _getProbability(currentContext, symbol);

    for(int depth = 0; depth < currentContext; depth++)
    {
        int fanout = 0;
        TrieNode* node = getTrieNode(depth, false);
        fanout = getFanout(node);

        int i = fanout >= F? F - 1 : fanout;
        int j = depth >= D? D - 1 : depth;
        double alpha = alpha_matrix[i][j];
        double beta = beta_matrix[i][j];

        double deltaAlpha = 0.00003d * calculateDerivativeAlpha(depth, symbol, alpha, beta);
        double deltaBeta = 0.00003d * calculateDerivativeBeta(depth, alpha, beta);

        if(!isnan(deltaAlpha) && !isinf(deltaAlpha)) alpha += deltaAlpha;
        if(!isnan(deltaBeta) && !isinf(deltaBeta)) beta += deltaBeta;

        if(beta > 0.99) beta = 0.99;
        if(beta < 0.1) beta = 0.1;
        if(alpha < -beta) alpha = -beta;

        // alpha_matrix[i][j] = alpha;
        // beta_matrix[i][j] = beta;
    }
}

double PPMModel::calculateDerivativeBeta(int depth, double alpha, double beta)
{
    if(depth == -1) return 0;

    TrieNode* node = getTrieNode(depth, false);
    ull tot = node->cum_count;
    int fanout = getFanout(node);

    double first = -1 + fanout * contextProbabilities[depth - 1];
    double second = (alpha + fanout * beta) * calculateDerivativeBeta(depth - 1, alpha, beta);

    return (first + second) / (contextProbabilities[depth] * (tot + alpha));
}

double PPMModel::calculateDerivativeAlpha(int depth, int symbol, double alpha, double beta)
{
    if(depth == -1) return 0;

    TrieNode* node = getTrieNode(depth, false);
    ull tot = node->cum_count;
    int fanout = getFanout(node);
    ull freq;
    if(node->trieNodes[symbol] != nullptr) freq = node->trieNodes[symbol]->frequency;
    else freq = 0;

    double first = (beta - freq + (tot - fanout * beta) * contextProbabilities[depth - 1]) / (tot + alpha);
    double second = (alpha + fanout * beta) * calculateDerivativeAlpha(depth - 1, symbol, alpha, beta);

    return (first + second) / (contextProbabilities[depth] * (tot + alpha));
}

PPMModel::TrieNode* PPMModel::getTrieNode(int depth, bool createAlongTheWay)
{
    TrieNode* node = trie;
    for(int j = 0; j < depth; j++)
    {
        int index = context->getByte(currentContext - depth + j);
        if(node->trieNodes[index] == nullptr)
        {
            if(createAlongTheWay)
                node->trieNodes[index] = new TrieNode();
            else
                return nullptr;
        }
        node = node->trieNodes[index];
    }
    return node;
}


PPMModel::~PPMModel()
{
    delete trie;
    delete context;
}


