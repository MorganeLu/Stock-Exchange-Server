DROP TABLE IF EXISTS OPEN_TABLE CASCADE;
CREATE TABLE OPEN_TABLE(
    ORDER_ID SERIAL NOT NULL,
    ACCOUNT_ID INT NOT NULL,
    STOCK_ID INT NOT NULL,
    AMOUNT INT NOT NULL,
    PRICE INT NOT NULL,
    TYPES INT NOT NULL,
    ORDER_TIME INT NOT NULL,
    PRIMARY KEY (ORDER_ID),
    FOREIGN KEY (ACCOUNT_ID) REFERENCES ACCOUNT(ACCOUNT_ID) ON DELETE CASCADE ON UPDATE CASCADE,
    FOREIGN KEY (STOCK_ID) REFERENCES STOCK(STOCK_ID) ON DELETE CASCADE ON UPDATE CASCADE
);