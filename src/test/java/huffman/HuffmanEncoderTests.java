package test.java.huffman;

import main.java.Symbol;
import main.java.huffman.HuffmanEncoder;
import org.junit.jupiter.api.Test;

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

        HuffmanEncoder encoder = new HuffmanEncoder();
        encoder.addSymbol(s0);
        encoder.addSymbol(s4);
        encoder.addSymbol(s2);
        encoder.addSymbol(s1);
        encoder.addSymbol(s3);

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

        HuffmanEncoder encoder = new HuffmanEncoder();
        encoder.addSymbol(s0);
        encoder.addSymbol(s1);
        encoder.addSymbol(s2);
        encoder.addSymbol(s3);
        encoder.addSymbol(s4);
        encoder.addSymbol(s5);
        encoder.addSymbol(s6);
        encoder.addSymbol(s7);
        encoder.addSymbol(s8);
        encoder.addSymbol(s9);
        encoder.addSymbol(s10);

        encoder.encode();

        assertEquals(3, s0.compressedLength);
        assertEquals(3, s1.compressedLength);
        assertEquals(3, s2.compressedLength);
        assertEquals(3, s3.compressedLength);
        assertEquals(3, s4.compressedLength);
        assertEquals(3, s5.compressedLength);
        assertEquals(4, s6.compressedLength);
        assertEquals(4, s7.compressedLength);
        assertEquals(4, s8.compressedLength);
        assertEquals(5, s9.compressedLength);
        assertEquals(5, s10.compressedLength);
    }

}
