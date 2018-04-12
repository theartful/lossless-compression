package test.java.huffman;

import main.java.Symbol;
import main.java.huffman.HuffmanEncoder;
import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;

/**
 * Created by theartful on 4/13/18.
 */
public class HuffmanTests {

    @Test
    public void testCase1() {
        Symbol s0 = new Symbol();
        Symbol s1 = new Symbol();
        Symbol s2 = new Symbol();
        Symbol s3 = new Symbol();
        Symbol s4 = new Symbol();

        HuffmanEncoder encoder = new HuffmanEncoder();
        encoder.addSymbol(s0, 32);
        encoder.addSymbol(s4, 11);
        encoder.addSymbol(s2, 16);
        encoder.addSymbol(s1, 27);
        encoder.addSymbol(s3, 14);

        encoder.encode();

        assertEquals(2, s0.compressedLength);
        assertEquals(2, s1.compressedLength);
        assertEquals(2, s2.compressedLength);
        assertEquals(3, s3.compressedLength);
        assertEquals(3, s4.compressedLength);
    }

}
