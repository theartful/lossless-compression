package main.java.huffman;

import main.java.Symbol;

import java.util.ArrayList;
import java.util.Comparator;
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
        symbolNodes = new ArrayList<HuffmanNode>();
        priorityQueue = new PriorityQueue<HuffmanNode>(new Comparator<HuffmanNode>() {
            @Override
            public int compare(HuffmanNode node1, HuffmanNode node2) {
                return (node1.frequency - node2.frequency < 0) ? -1 : 1;
            }
        });
    }

    public boolean addSymbol(Symbol symbol, long frequency) {
        if (symbol == null) return false;
        if (frequency <= 0) return false;
        HuffmanNode node = new HuffmanNode(symbol, frequency);
        return symbolNodes.add(node) && priorityQueue.add(node);
    }

    public void encode() {
        long length = symbolNodes.size();
        HuffmanNode node = null;
        for (int i = 0; i < length - 1; i++) {
            node = new HuffmanNode();
            node.setLeft(priorityQueue.poll());
            node.setRight(priorityQueue.poll());
            node.frequency = getFrequency(node.left) + getFrequency(node.right);
            priorityQueue.add(node);
        }
        priorityQueue.poll();
        if (!priorityQueue.isEmpty()) {
            System.out.println("HuffmanEncoder: Error! priorityQueue should be empty");
        }
        root = node;
    }

    private long getFrequency(HuffmanNode node) {
        if (node == null) return 0;
        return node.frequency;
    }

}
