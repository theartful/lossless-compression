package main.java.huffman;

import main.java.Symbol;

import java.io.InputStream;
import java.util.ArrayList;
import java.util.BitSet;
import java.util.List;

/**
 * Created by theartful on 4/13/18.
 */
public class HuffmanAnalyzer {

    private List<Symbol> symbols;
    private int symbolLength;

    public boolean analyze(InputStream inputStream, int blockSize) {
        if (inputStream == null || symbols == null) return false;

        int blockSizeInBytes = (int) ((long) symbolLength * blockSize) / 8;

        byte[] block = new byte[blockSizeInBytes];
        // number of read symbols of the stream
        int actualBytes = 0;

        try {
            actualBytes = inputStream.read(block, 0, blockSizeInBytes);
        } catch (Exception e) {
            e.printStackTrace();
            return false;
        }

        int index = 0;
        int m = 0;
        for (int i = 0; i < actualBytes; i++) {
            byte b = block[i];
            for (int j = 0; j < 8; j++) {
                if ((b & 0b10000000) != 0) index++;
                if ((m + 1) % symbolLength == 0) {
                    symbols.get(index).frequency++;
                    m -= symbolLength;
                    index = 0;
                }
                index = (index << 1);
                b *= 2;
                m++;
            }
        }
        return true;
    }

    public void createSymbols(int symbolLength) {
        this.symbolLength = symbolLength;
        symbols = new ArrayList<>();
        int x = 0;
        int symbolsNumber = 1 << (symbolLength);
        for (int i = 0; i < symbolsNumber; i++) {
            Symbol symbol = new Symbol();
            symbol.length = symbolLength;
            BitSet bitset = new BitSet();
            int z = x;
            for (int j = 0; z != 0; j++) {
                if (z % 2 != 0) bitset.set(j);
                z /= 2;
            }
            symbol.symbolCode = bitset;
            symbols.add(symbol);
            x++;
        }
    }

    public List<Symbol> getSymbols() {
        return symbols;
    }
}
