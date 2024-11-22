import React from 'react';

import * as styles from './header.scss';
const Header = (): React.JSX.Element => {
    return (
        <header className={styles.header}>
            <h1>Bob</h1>
        </header>
    );
};

export default Header;
