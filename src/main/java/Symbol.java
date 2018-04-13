package main.java;

import java.util.BitSet;

/**
 * Created by theartful on 4/12/18.
 */
public class Symbol {
    // number of bytes
    public int length;
    // least significant bit is at bit 0
    public BitSet symbolCode;
    public int compressedLength;
    public BitSet compressedSymbolCode;
    public long frequency;

    public Symbol() {

    }

    public Symbol(int length, BitSet symbolCode) {
        this.length = length;
        this.symbolCode = symbolCode;
    }

    public Symbol(long frequency) {
        this.frequency = frequency;
    }

}
