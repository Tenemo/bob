import { Button, CircularProgress } from '@mui/material';
import React from 'react';

import { useAppSelector } from 'app/hooks';
import { selectActiveMovement } from 'features/Bob/bobSlice';
import {
    MoveCommandRequest,
    useMoveCommandMutation,
} from 'features/BobApi/bobApi';

type MovementButtonProps = {
    type: MoveCommandRequest['type'];
    children: React.ReactNode;
};

const MovementButton = ({
    type,
    children,
}: MovementButtonProps): React.JSX.Element => {
    const [moveCommand, { isLoading: isMoveLoading }] = useMoveCommandMutation({
        fixedCacheKey: 'moveCommand',
    });

    const activeMovement = useAppSelector(selectActiveMovement);
    const isButtonActive = activeMovement === type;

    const handleClick = (): void => {
        void moveCommand({ type });
    };

    return (
        <Button
            onClick={handleClick}
            sx={{
                '&.Mui-disabled': {
                    background: isButtonActive ? '#002101' : undefined,
                },
                flex: 1,
            }}
        >
            {children}{' '}
            {isMoveLoading && isButtonActive && (
                <CircularProgress size={20} sx={{ position: 'absolute' }} />
            )}
        </Button>
    );
};

export default MovementButton;
