import { FormControlLabel, Switch } from '@mui/material';
import React from 'react';

import styles from './footer.scss';

import { useAppDispatch, useAppSelector } from 'app/hooks';
import {
    setDebugMode,
    selectIsDebug,
    selectUseBobSpeaker,
    setUseBobSpeaker,
} from 'features/Bob/bobSlice';

const Footer = (): React.JSX.Element => {
    const dispatch = useAppDispatch();
    const isDebug = useAppSelector(selectIsDebug);
    const useBobSpeaker = useAppSelector(selectUseBobSpeaker);

    return (
        <footer className={styles.footer}>
            <div className={styles.toggleButton}>
                <FormControlLabel
                    control={
                        <Switch
                            checked={useBobSpeaker}
                            onChange={(e): void =>
                                void dispatch(
                                    setUseBobSpeaker(e.target.checked),
                                )
                            }
                        />
                    }
                    label={"Output audio on Bob's speaker"}
                />
            </div>
            <div className={styles.toggleButton}>
                <FormControlLabel
                    control={
                        <Switch
                            checked={isDebug}
                            onChange={(e): void => {
                                void dispatch(setDebugMode(e.target.checked));
                            }}
                        />
                    }
                    label="Debug mode"
                />
            </div>
        </footer>
    );
};

export default Footer;
