import { FormControlLabel, Switch } from '@mui/material';
import React from 'react';

import styles from './footer.scss';

import { useAppDispatch, useAppSelector } from 'app/hooks';
import { setDebugMode, selectIsDebug } from 'features/Bob/bobSlice';

const Footer = (): React.JSX.Element => {
    const dispatch = useAppDispatch();
    const isDebug = useAppSelector(selectIsDebug);

    return (
        <footer className={styles.footer}>
            <FormControlLabel
                control={
                    <Switch
                        checked={isDebug}
                        onChange={(e): void => {
                            dispatch(setDebugMode(e.target.checked));
                        }}
                    />
                }
                label="Debug mode"
            />
        </footer>
    );
};

export default Footer;
