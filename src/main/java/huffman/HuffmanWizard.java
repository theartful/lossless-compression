package main.java.huffman;

import main.java.Symbol;

import java.io.*;
import java.util.List;

/**
 * Created by theartful on 4/13/18.
 */
public class HuffmanWizard {

    public static void encode(FileInputStream src, FileOutputStream dist, int wordLength, int blockSize) throws IOException {
        HuffmanEncoder encoder = new HuffmanEncoder();
        List<Symbol> symbols = BlockEncoderDecoder.createSymbols(wordLength);
        encoder.setSymbols(symbols);

        Block block;
        while ((block = BlockEncoderDecoder.analyzeBlock(src, blockSize, symbols, wordLength)) != null) {
            encoder.encode();

            // temporary
            for (Symbol symbol : symbols) {
                for (int i = 0; i < symbol.compressedLength / 2; i++) {
                    int a = (symbol.compressedSymbolCode & (1 << i)) != 0 ? 1 : 0;
                    int b = (symbol.compressedSymbolCode & (1 << (symbol.compressedLength - i - 1))) != 0 ? 1 : 0;

                    if (a == 1) symbol.compressedSymbolCode |= (1 << (symbol.compressedLength - i - 1));
                    else symbol.compressedSymbolCode &= ~(1 << (symbol.compressedLength - i - 1));
                    if (b == 1) symbol.compressedSymbolCode |= (1 << i);
                    else symbol.compressedSymbolCode &= ~(1 << i);
                }
            }
            BlockEncoderDecoder.encodeBlock(dist, block);
        }
    }

    public static void decodeFile(FileInputStream src, FileOutputStream dist, int wordLength) throws IOException {
        List<Symbol> symbols = BlockEncoderDecoder.createSymbols(wordLength);
        while (BlockEncoderDecoder.decodeBlock(src, dist, symbols)) ;
    }

}
