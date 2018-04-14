package main.java;

/**
 * Created by theartful on 4/12/18.
 */

public class Symbol {

    // number of bytes
    public int length;
    // least significant bit is at bit 0
    public int symbolCode;
    public int compressedLength;
    public long compressedSymbolCode;
    public int frequency;

    public Symbol() {

    }

    public Symbol(int frequency) {
        this.frequency = frequency;
    }
}
