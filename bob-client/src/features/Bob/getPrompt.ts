import markdownPrompts from 'prompts.md';

export const getPrompt = (heading: string): string => {
    const regex = new RegExp(`####\\s*${heading}\\s*([^#]+)`, 'i');
    const match = markdownPrompts.match(regex);
    return match ? match[1].replace(/\n/g, ' ').trim() : '';
};
