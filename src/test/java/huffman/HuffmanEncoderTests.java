package test.java.huffman;

import main.java.Symbol;
import main.java.huffman.HuffmanEncoder;

import org.junit.jupiter.api.Test;

import java.util.ArrayList;
import java.util.List;

import static org.junit.jupiter.api.Assertions.assertEquals;

/**
 * Created by theartful on 4/13/18.
 */

class HuffmanEncoderTests {

    @Test
    void testCase1() {
        Symbol s0 = new Symbol(32);
        Symbol s1 = new Symbol(27);
        Symbol s2 = new Symbol(16);
        Symbol s3 = new Symbol(14);
        Symbol s4 = new Symbol(11);
        List<Symbol> symbols = new ArrayList<>();
        symbols.add(s0);
        symbols.add(s1);
        symbols.add(s2);
        symbols.add(s3);
        symbols.add(s4);

        HuffmanEncoder encoder = new HuffmanEncoder();
        encoder.setSymbols(symbols);

        encoder.encode();

        assertEquals(2, s0.compressedLength);
        assertEquals(2, s1.compressedLength);
        assertEquals(2, s2.compressedLength);
        assertEquals(3, s3.compressedLength);
        assertEquals(3, s4.compressedLength);
    }

    @Test
    void testCase2() {
        Symbol s0 = new Symbol(15);
        Symbol s1 = new Symbol(14);
        Symbol s2 = new Symbol(12);
        Symbol s3 = new Symbol(10);
        Symbol s4 = new Symbol(10);
        Symbol s5 = new Symbol(10);
        Symbol s6 = new Symbol(8);
        Symbol s7 = new Symbol(7);
        Symbol s8 = new Symbol(6);
        Symbol s9 = new Symbol(5);
        Symbol s10 = new Symbol(3);
        List<Symbol> symbols = new ArrayList<>();

        symbols.add(s0);
        symbols.add(s1);
        symbols.add(s2);
        symbols.add(s3);
        symbols.add(s4);
        symbols.add(s5);
        symbols.add(s6);
        symbols.add(s7);
        symbols.add(s8);
        symbols.add(s9);
        symbols.add(s10);

        HuffmanEncoder encoder = new HuffmanEncoder();
        encoder.setSymbols(symbols);

        encoder.encode();

        assertEquals(3, s0.compressedLength);
        assertEquals(0b000, s0.compressedSymbolCode);

        assertEquals(3, s1.compressedLength);
        assertEquals(0b001, s1.compressedSymbolCode);

        assertEquals(3, s2.compressedLength);
        assertEquals(0b010, s2.compressedSymbolCode);

        assertEquals(3, s3.compressedLength);
        assertEquals(0b011, s3.compressedSymbolCode);

        assertEquals(3, s4.compressedLength);
        assertEquals(0b100, s4.compressedSymbolCode);

        assertEquals(3, s5.compressedLength);
        assertEquals(0b101, s5.compressedSymbolCode);

        assertEquals(4, s6.compressedLength);
        assertEquals(0b1100, s6.compressedSymbolCode);

        assertEquals(4, s7.compressedLength);
        assertEquals(0b1101, s7.compressedSymbolCode);

        assertEquals(4, s8.compressedLength);
        assertEquals(0b1110, s8.compressedSymbolCode);

        assertEquals(5, s9.compressedLength);
        assertEquals(0b11110, s9.compressedSymbolCode);

        assertEquals(5, s10.compressedLength);
        assertEquals(0b11111, s10.compressedSymbolCode);
    }

}
