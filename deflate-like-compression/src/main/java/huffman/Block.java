package main.java.huffman;

import main.java.Symbol;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by theartful on 4/13/18.
 */
public class Block {

    public int literalSymbolsLength;
    public Symbol eobSymbol;
    List<Symbol> symbols;
    List<Integer> symbolsSequence;

    public Block() {
        symbolsSequence = new ArrayList<>();
    }

    public void addSymbol(int index) {
        symbolsSequence.add(index);
    }

    public void setSymbols(List<Symbol> symbols) {
        literalSymbolsLength = symbols.size() - 1;
        this.symbols = symbols;
        eobSymbol = symbols.get(symbols.size() - 1);
    }

}
