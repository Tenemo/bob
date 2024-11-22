import { Button, Typography, Box } from '@mui/material';
import { RealtimeClient } from '@openai/realtime-api-beta';
import React, { useState, useEffect, useRef, useCallback } from 'react';

import { useAppSelector } from 'app/hooks';
import { selectBobConnectionStatus } from 'features/Bob/bobSlice';
import { useUploadAudioMutation } from 'features/BobApi/bobApi';

const Realtime = (): React.JSX.Element => {
    return <>OpenAI API stuff</>;
};

export default Realtime;
