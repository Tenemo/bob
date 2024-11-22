import { createApi, fetchBaseQuery } from '@reduxjs/toolkit/query/react';
import type {
    BaseQueryFn,
    FetchArgs,
    FetchBaseQueryError,
} from '@reduxjs/toolkit/query/react';

import type { RootState } from 'app/store';
import { selectBobIp } from 'features/Bob/bobSlice';

export type HealthcheckResponse = {
    status: string;
};

const rawBaseQuery = fetchBaseQuery({
    baseUrl: '',
});

const dynamicBaseQuery: BaseQueryFn<
    string | FetchArgs,
    unknown,
    FetchBaseQueryError
> = async (args, api, extraOptions) => {
    const ip = selectBobIp(api.getState() as RootState);
    if (!ip) {
        return {
            error: {
                status: 400,
                statusText: 'Bad Request',
                data: 'No IP address provided',
            },
        };
    }

    const urlEnd = typeof args === 'string' ? args : args.url;
    const adjustedUrl = `http://${ip}/${urlEnd}`;
    const adjustedArgs =
        typeof args === 'string' ? adjustedUrl : { ...args, url: adjustedUrl };

    return rawBaseQuery(adjustedArgs, api, extraOptions);
};

export const bobApi = createApi({
    reducerPath: 'bobApi',
    baseQuery: dynamicBaseQuery,
    endpoints: (builder) => ({
        /**
         * Health check query.
         */
        healthcheck: builder.query<HealthcheckResponse, void>({
            query: () => `health-check`,
        }),
        /**
         * Capture photo query.
         */
        capture: builder.query<Blob, void>({
            query: () => ({
                url: `capture`,
                method: 'GET',
                // Explicitly type the response parameter
                responseHandler: (response: Response) => response.blob(),
            }),
        }),
    }),
});

export const { useHealthcheckQuery, useCaptureQuery } = bobApi;
