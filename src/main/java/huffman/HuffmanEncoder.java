package main.java.huffman;

import main.java.Symbol;

import java.util.ArrayList;
import java.util.List;
import java.util.PriorityQueue;

/**
 * Created by theartful on 4/12/18.
 */

public class HuffmanEncoder {

    HuffmanNode root;
    List<HuffmanNode> symbolNodes;
    PriorityQueue<HuffmanNode> priorityQueue;

    public HuffmanEncoder() {
        symbolNodes = new ArrayList<>();
        priorityQueue = new PriorityQueue<>((node1, node2) ->
                (node1.getFrequency() - node2.getFrequency() < 0) ? -1 : 1);
    }

    public boolean addSymbol(Symbol symbol) {
        if (symbol == null) return false;
        HuffmanNode node = new HuffmanNode(symbol);
        return symbolNodes.add(node) && priorityQueue.add(node);
    }

    public void encode() {
        long length = symbolNodes.size();
        HuffmanNode node = null;
        for (int i = 0; i < length - 1; i++) {
            node = new HuffmanNode();
            node.setLeft(priorityQueue.poll());
            node.setRight(priorityQueue.poll());
            node.setFrequency(getFrequency(node.left) + getFrequency(node.right));
            priorityQueue.add(node);
        }
        priorityQueue.poll();
        if (!priorityQueue.isEmpty()) {
            System.out.println("HuffmanEncoder: Error! priorityQueue should be empty");
        }
        root = node;
        assignCodes();
    }

    private void assignCodes() {
    }

    private long getFrequency(HuffmanNode node) {
        if (node == null) return 0;
        return node.getFrequency();
    }

}
