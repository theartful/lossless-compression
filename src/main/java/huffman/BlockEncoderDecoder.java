package main.java.huffman;

import main.java.Symbol;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.List;

/**
 * Created by theartful on 4/13/18.
 */
public class BlockEncoderDecoder {


    private static final int ADDRESS_LENGTH = 5;

    public static void encodeBlock(OutputStream os, Block block) throws IOException {
        int numberOfBytes = 0;

        // calculate number of bytes
        for (int symbolIndex : block.symbolsSequence) {
            numberOfBytes += block.symbols.get(symbolIndex).compressedLength;
        }
        numberOfBytes += block.eobSymbol.compressedLength;

        numberOfBytes += block.symbols.size() * (ADDRESS_LENGTH);
        numberOfBytes = (int) Math.ceil(numberOfBytes / 8.0f);

        byte[] bytes = new byte[numberOfBytes];
        int[] byteAndBitIndices = new int[2];

        // encodeBlock header
        for (Symbol symbol : block.symbols) {
            writeSymbol(bytes, symbol.compressedLength, ADDRESS_LENGTH, byteAndBitIndices);
        }

        // encodeBlock body
        for (int symbolIndex : block.symbolsSequence) {
            Symbol symbol = block.symbols.get(symbolIndex);
            // add symbol
            writeSymbol(bytes, symbol.compressedSymbolCode, symbol.compressedLength, byteAndBitIndices);
        }
        writeSymbol(bytes, block.eobSymbol.compressedSymbolCode, block.eobSymbol.compressedLength, byteAndBitIndices);
        os.write(bytes);
    }


    public static Block analyzeBlock(InputStream inputStream, int blockSize, List<Symbol> symbols, int symbolLength) {
        if (inputStream == null || symbols == null) return null;

        int blockSizeInBytes = (int) ((long) symbolLength * blockSize) / 8;

        byte[] data = new byte[blockSizeInBytes];

        // number of read symbols of the stream
        int actualBytes = 0;

        Block block = new Block();
        block.setSymbols(symbols);
        clearFrequencies(symbols);

        try {
            actualBytes = inputStream.read(data, 0, blockSizeInBytes);
            if (actualBytes <= 0) return null;
        } catch (Exception e) {
            e.printStackTrace();
            return null;
        }

        int index = 0;
        int m = 0;
        for (int i = 0; i < actualBytes; i++) {
            int b = data[i];

            for (int j = 0; j < 8; j++) {
                if ((b & 1) != 0) index |= (1 << m);
                if ((m + 1) % symbolLength == 0) {
                    symbols.get(index).frequency++;
                    block.addSymbol(index);
                    m -= symbolLength;
                    index = 0;
                }
                b = (b >> 1);
                m++;
            }

        }
        if (m != 0) {
            block.addSymbol(index);
        }
        return block;
    }

    public static boolean decodeBlock(InputStream is, OutputStream os, List<Symbol> symbols) throws IOException {
        int[] byteAndBitIndicies = new int[2];
        int headerBytesNumber = (int) Math.ceil(ADDRESS_LENGTH * symbols.size() / 8.0f);
        byte[] headerBytes = new byte[headerBytesNumber];

        if (is.read(headerBytes) != headerBytesNumber) {
            return false;
        }

        for (int i = 0; i < symbols.size(); i++) {
            int code = readCode(headerBytes, byteAndBitIndicies, ADDRESS_LENGTH);
            symbols.get(i).compressedLength = code;
        }

        Block block = new Block();
        block.setSymbols(symbols);
        HuffmanEncoder.assignCodes(symbols);

        HuffmanNode root = HuffmanEncoder.buildTree(symbols);
        HuffmanNode node = root;

        if (byteAndBitIndicies[0] < headerBytesNumber) {
            for (int i = byteAndBitIndicies[1]; i < 8; i++) {
                if ((headerBytes[byteAndBitIndicies[0]] & (1 << i)) != 0) {
                    node = node.right;
                } else {
                    node = node.left;
                }
                if (node.symbol != null) {
                    if (node.symbol == block.eobSymbol) {
                        return writeBlock(os, block);
                    }
                    block.addSymbol(node.symbol.symbolCode);
                    node = root;
                }
            }
        }

        int b;
        while ((b = is.read()) != -1) {
            for (int j = 0; j < 8; j++) {
                if ((b & (1 << j)) != 0) {
                    node = node.right;
                } else {
                    node = node.left;
                }
                if (node.symbol != null) {
                    if (node.symbol == block.eobSymbol) {
                        return writeBlock(os, block);
                    }
                    block.addSymbol(node.symbol.symbolCode);
                    node = root;
                }
            }
        }
        return false;
    }

    private static boolean writeBlock(OutputStream os, Block block) throws IOException {
        int numberOfBytes = 0;
        for (int index : block.symbolsSequence) {
            numberOfBytes += block.symbols.get(index).length;
        }
        numberOfBytes = (int) Math.ceil(numberOfBytes / 8.0f);
        byte[] bytes = new byte[numberOfBytes];
        int[] byteAndBitIndices = new int[2];
        for (int index : block.symbolsSequence) {
            Symbol symbol = block.symbols.get(index);
            writeSymbol(bytes, symbol.symbolCode, symbol.length, byteAndBitIndices);
        }
        os.write(bytes);
        return true;
    }


    private static void writeSymbol(byte[] bytes, long code, int codeLength, int[] byteAndBitIndices) {
        int byteIndex = byteAndBitIndices[0];
        int bitIndex = byteAndBitIndices[1];

        for (int i = 0; i < codeLength; i++) {
            if ((code & (1 << i)) != 0) {
                bytes[byteIndex] |= (1 << bitIndex);
            }
            bitIndex++;
            if (bitIndex > 7) {
                bitIndex = 0;
                byteIndex++;
            }
        }

        byteAndBitIndices[0] = byteIndex;
        byteAndBitIndices[1] = bitIndex;
    }

    private static int readCode(byte[] bytes, int[] byteAndBitIndices, int length) {
        int code = 0;
        int byteIndex = byteAndBitIndices[0];
        int bitIndex = byteAndBitIndices[1];
        for (int j = 0; j < length; j++) {
            if ((bytes[byteIndex] & (1 << bitIndex)) != 0) {
                code |= (1 << j);
            }
            bitIndex++;
            if (bitIndex > 7) {
                bitIndex = 0;
                byteIndex++;
            }
        }
        byteAndBitIndices[0] = byteIndex;
        byteAndBitIndices[1] = bitIndex;
        return code;
    }

    private static void clearFrequencies(List<Symbol> symbols) {
        for (Symbol symbol : symbols) {
            symbol.frequency = 0;
            symbol.compressedLength = 0;
        }
        symbols.get(symbols.size() - 1).frequency = 1;
    }

    public static List<Symbol> createSymbols(int symbolLength) {
        List<Symbol> symbols = new ArrayList<>();
        int x = 0;
        int symbolsNumber = 1 << (symbolLength);
        for (int i = 0; i < symbolsNumber; i++) {
            Symbol symbol = new Symbol();
            symbol.length = symbolLength;
            symbol.symbolCode = x;
            symbols.add(symbol);
            x++;
        }
        Symbol endOfBlockSymbol = new Symbol();
        endOfBlockSymbol.frequency = 1;
        symbols.add(endOfBlockSymbol);
        return symbols;
    }


}
