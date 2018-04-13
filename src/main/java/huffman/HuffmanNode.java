package main.java.huffman;

import main.java.Symbol;

/**
 * Created by theartful on 4/12/18.
 */
public class HuffmanNode {

    Symbol symbol;
    private long frequency;
    int depth;

    HuffmanNode left;
    HuffmanNode right;

    public HuffmanNode(Symbol symbol) {
        this.symbol = symbol;
        if (symbol != null)
            this.frequency = symbol.frequency;
        this.depth = 0;
    }

    public HuffmanNode() {

    }

    public void setLeft(HuffmanNode node) {
        this.left = node;
        if (left != null)
            left.increaseDepth();
    }

    public void setRight(HuffmanNode node) {
        this.right = node;
        if (right != null)
            right.increaseDepth();
    }

    public HuffmanNode getLeft() {
        return left;
    }

    public HuffmanNode getRight() {
        return right;
    }

    public void increaseDepth() {
        depth++;
        if (symbol != null) {
            symbol.compressedLength = depth;
        }
        if (left != null) left.increaseDepth();
        if (right != null) right.increaseDepth();
    }

    public void setFrequency(long frequency) {
        this.frequency = frequency;
        if (symbol != null)
            symbol.frequency = frequency;
    }

    public long getFrequency() {
        return frequency;
    }

}
