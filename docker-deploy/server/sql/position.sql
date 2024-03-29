DROP TABLE IF EXISTS POSITION CASCADE;
CREATE TABLE POSITION(
    ID SERIAL NOT NULL,
    STOCK_ID INT NOT NULL,
    ACCOUNT_ID INT NOT NULL,
    AMOUNT FLOAT NOT NULL,
    PRIMARY KEY (ID),
    FOREIGN KEY (ACCOUNT_ID) REFERENCES ACCOUNT(ACCOUNT_ID) ON DELETE CASCADE ON UPDATE CASCADE,
    FOREIGN KEY (STOCK_ID) REFERENCES STOCK(STOCK_ID) ON DELETE CASCADE ON UPDATE CASCADE
);