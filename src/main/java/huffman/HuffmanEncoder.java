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
    List<Symbol> symbols;

    public HuffmanEncoder() {
    }

    public void setSymbols(List<Symbol> symbols) {
        this.symbols = symbols;
        if (symbols == null) return;
    }

    private void addSymbol(Symbol symbol) {
        if (symbol == null) return;
        if (symbol.frequency == 0) return;
        HuffmanNode node = new HuffmanNode(symbol);
        symbolNodes.add(node);
        priorityQueue.add(node);
    }

    public void encode() {
        symbolNodes = new ArrayList<>();
        priorityQueue = new PriorityQueue<>(Comparator.comparing(node -> node.getFrequency()));
        for (Symbol symbol : symbols) {
            addSymbol(symbol);
        }

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
        assignCodes(symbols);
    }

    public static void assignCodes(List<Symbol> symbols) {
        int maxCodeLength = 0;
        for (Symbol symbol : symbols) {
            if (symbol.compressedLength > maxCodeLength)
                maxCodeLength = symbol.compressedLength;
        }

        int[] l_count = new int[maxCodeLength + 1];
        l_count[0] = 0;

        for (Symbol symbol : symbols) {
            if (symbol.compressedLength == 0) continue;
            l_count[symbol.compressedLength]++;
        }

        long[] next_code = new long[maxCodeLength + 1];
        long code = 0;
        for (int bits = 1; bits <= maxCodeLength; bits++) {
            code = (code + l_count[bits - 1]) << 1;
            next_code[bits] = code;
        }

        for (Symbol symbol : symbols) {
            if (symbol.compressedLength != 0) {
                symbol.compressedSymbolCode = next_code[symbol.compressedLength];
                next_code[symbol.compressedLength]++;
            }
        }

    }

    private int getFrequency(HuffmanNode node) {
        if (node == null) return 0;
        return node.getFrequency();
    }

    public static HuffmanNode buildTree(List<Symbol> symbols) {

        HuffmanNode root = new HuffmanNode();
        for (Symbol symbol : symbols) {
            if (symbol.compressedLength == 0) continue;
            HuffmanNode node = root;
            long code = symbol.compressedSymbolCode;
            for (int j = symbol.compressedLength - 1; j >= 0; j--) {
                if ((code & (1 << j)) != 0) {
                    if (node.right == null)
                        node.right = new HuffmanNode();
                    node = node.right;
                } else {
                    if (node.left == null)
                        node.left = new HuffmanNode();
                    node = node.left;
                }
            }
            node.symbol = symbol;
        }
        return root;
    }
}
